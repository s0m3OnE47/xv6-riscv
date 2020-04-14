//
// network system calls.
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "net.h"

struct sock {
  struct sock *next; // the next socket in the list
  uint32 raddr;      // the remote IPv4 address
  uint16 lport;      // the local UDP port number
  uint16 rport;      // the remote UDP port number
  struct spinlock lock; // protects the rxq
  struct mbufq rxq;  // a queue of packets waiting to be received
};

static struct spinlock lock;
static struct sock *sockets;

void
sockinit(void)
{
  initlock(&lock, "socktbl");
}

int
sockalloc(struct file **f, uint32 raddr, uint16 lport, uint16 rport)
{
  struct sock *si, *pos;

  si = 0;
  *f = 0;
  if ((*f = filealloc()) == 0)
    goto bad;
  if ((si = (struct sock*)kalloc()) == 0)
    goto bad;

  // initialize objects
  si->raddr = raddr;
  si->lport = lport;
  si->rport = rport;
  initlock(&si->lock, "sock");
  mbufq_init(&si->rxq);
  (*f)->type = FD_SOCK;
  (*f)->readable = 1;
  (*f)->writable = 1;
  (*f)->sock = si;

  // add to list of sockets
  acquire(&lock);
  pos = sockets;
  while (pos) {
    if (pos->raddr == raddr &&
        pos->lport == lport &&
	pos->rport == rport) {
      release(&lock);
      goto bad;
    }
    pos = pos->next;
  }
  si->next = sockets;
  sockets = si;
  release(&lock);
  return 0;

bad:
  if (si)
    kfree((char*)si);
  if (*f)
    fileclose(*f);
  return -1;
}

//
// Add and wire in methods to handle closing, reading,
// and writing for network sockets.
//

// called by protocol handler layer to deliver UDP packets
void
sockrecvudp(struct mbuf *m, uint32 raddr, uint16 lport, uint16 rport)
{
  //
  // Find the socket that handles this mbuf and deliver it, waking
  // any sleeping reader. Free the mbuf if there are no sockets
  // registered to handle it.
  //
  struct sock *sock = 0;
  for(sock = sockets; sock; sock=sock->next){
    if(sock->rport==rport && sock->lport==lport && sock->raddr == raddr){
      mbufq_pushtail(&sock->rxq, m);
      wakeup(&sock->rxq);
    }
  }
}

void sockclose(struct sock* sock){
  acquire(&sock->lock);
  acquire(&lock);
  struct sock* cur = sockets;
  struct sock* prev = 0;
  while(cur){
    if(cur == sock){
      if(prev){
        acquire(&prev->lock);
        prev->next = cur->next;
        release(&prev->lock);
        break;
      }else{
        sockets = sockets->next;
        break;
      }
      kfree(sock);
    }
    prev = cur;
    cur = cur->next;
  }
  release(&lock);
  while(!mbufq_empty(&sock->rxq)){
    struct mbuf * m = mbufq_pophead(&sock->rxq);
    mbuffree(m);
  }
  release(&sock->lock);
}

int sockwrite(struct sock *sock, uint64 addr, int n) {
  // udp packet size must be smaller than MTU, no need loop
  struct proc *pr = myproc();
  acquire(&sock->lock);
  int nwrite = 0;
  int headroom = sizeof(struct udp) + sizeof(struct ip) + sizeof(struct eth);
  struct mbuf *buf = mbufalloc(headroom);               // Headroom for the UDP, IP, and Ethernet headers
  uint64 size = n > (MBUF_SIZE - headroom) ? (MBUF_SIZE - headroom) : n;
  if(copyin(pr->pagetable, buf->head, addr, size) == -1){
    release(&sock->lock);
    return -1;
  }
  nwrite += size;
  buf->len = size;
  net_tx_udp(buf, sock->raddr, sock->lport, sock->rport);
  release(&sock->lock);
  return nwrite;
}

int sockread(struct sock *sock, uint64 addr, int n) {
  // udp packet size must be smaller than MTU, no need loop
  struct proc *pr = myproc();
  acquire(&sock->lock);
  struct mbuf *buf = 0;
  int nread = 0;
  // wait for rx ready
  while (mbufq_empty(&sock->rxq)) {             // Check if rxq is empty
    if (myproc()->killed) {
      release(&sock->lock);
      return -1;
    }
    sleep(&sock->rxq, &sock->lock);
  }
  buf = mbufq_pophead(&sock->rxq);              // Pop the mbuf from rxq
  uint64 size = n > buf->len ? buf->len : n;
  if(copyout(pr->pagetable, addr, buf->head, size) == -1){    // Move its payload into user memory
    release(&sock->lock);
    return -2;
  }
  nread += size;
  mbuffree(buf);
  release(&sock->lock);
  return nread;
}
