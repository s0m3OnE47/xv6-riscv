# Modified xv6 Kernel

Complied xv6 operating system for RISC-V architecture. Ran it on QEMU an open source machine emulator and virtualizer. Wrote code to implement the following:

- Utilities
- Lazy Page Allocation
- Copy-on-Write Fork
- Uthread
- Alarm
- Lock
- File System
- Network Driver

## Utilities

This made me familiar with system calls. Wrote five userspace programs to interact with the kernel.

    sleep: Pauses for specified number of ticks.
    Pingpong: Transfers a byte between two processes over a pair of pipes, one for each direction.
    Primes : A concurrent version of prime sieve number geneation using pipes. Invented by C. A. R. Hoare.
    find: A find program find files in xv6 operating system.
    xargs: A version of UNIX program that read lines from standard input and run a command for each line, supplying the line as arguments to the command.

## Lazy Page Allocation

This deals with user-space heap memory of page table by lazy page allocation. There are programs that ask for large amounts of memory but never use most of it, for example to implement large sparse arrays. To optimize for this case, sophisticated kernels allocate user memory lazily. That is, sbrk() doesn't allocate physical memory, but just remembers which addresses are allocated. When the process first tries to use any given page of memory, the CPU generates a page fault, which the kernel handles by allocating physical memory, zeroing it, and mapping it.

## Copy-on-Write Fork

The task is to implement copy-on-write fork in the xv6 kernel. The fork() system call in xv6 copies all of the parent process's user-space memory into the child. If the parent is large, copying can take a long time, often wasting memory. Copy-on-write fork() is to defer allocating and copying physical memory pages for the child until the copies are actually needed, if ever. When either parent or child process tries to write one of these pages, the CPU will force a page fault. The kernel page-fault handler detects this case, allocates a page of physical memory for the faulting process, copies the original page into the new page, and modifies the relevant page table entry (PTE) in the faulting process to refer to the new page, this time with the PTE marked writeable. When the page fault handler returns, the user process will be able to write its copy of the page.

## Uthread

Designed the context switch mechanism for a user-level threading system, i.e create threads and save/restore registers to switch between threads.

## Alarm

It periodically alerts a process as it uses CPU time. Added a new sigalarm() system call. If an application calls sigalarm(n, fn), then after every n "ticks" of CPU time that the program consumes, the kernel causes application function fn to be called. When fn returns, the application resumes where it left off. A tick is a fairly arbitrary unit of time in xv6, determined by how often a hardware timer generates interrupts.

## Lock

Implemented a free list per CPU, each list with its own lock. Allocations and frees on different CPUs can run in parallel, because each CPU will operate on a different list. This deals with the case in which one CPU's free list is empty, but another CPU's list has free memory; in that case, the one CPU "steals" part of the other CPU's free list. Stealing may introduce lock contention, but that will hopefully be infrequent.

## File System

Implemented support for large files and symbolic links to the xv6 file system. Increased file size from 256 blocks (256 kB) to 65803 blocks (65803 kB).

Implemented the symlink() system call, which creates a new symbolic link at linkpath that refers to file named by target

## Network Driver

Wrote and modified a device driver for a network interface card (NIC) and added support for UDP network sockets to xv6.

Implemented E1000 networking driver to send and receive packets.

Network sockets are a standard abstraction for OS networking that bear similarity to files. Sockets are accessed through ordinary file descriptors (just like files, pipes, and devices). Reading from a socket file descriptor receives a packet while writing to it sends a packet. If no packets are currently available to be received, it blocks and wait for the next packet to arrive (i.e. allow rescheduling to another process). Implemented a stripped down version of sockets that supports the UDP network protocol.

- - -
xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)). See also https://pdos.csail.mit.edu/6.828/, which
provides pointers to on-line resources for v6.

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by
Silas Boyd-Wickizer, Anton Burtsev, Dan Cross, Cody Cutler, Mike CAT,
Tej Chajed, eyalz800, Nelson Elhage, Saar Ettinger, Alice Ferrazzi,
Nathaniel Filardo, Peter Froehlich, Yakir Goaron,Shivam Handa, Bryan
Henry, Jim Huang, Alexander Kapshuk, Anders Kaseorg, kehao95, Wolfgang
Keller, Eddie Kohler, Austin Liew, Imbar Marinescu, Yandong Mao, Matan
Shabtay, Hitoshi Mitake, Carmi Merimovich, Mark Morrissey, mtasm, Joel
Nider, Greg Price, Ayan Shafqat, Eldar Sehayek, Yongming Shen, Cam
Tenny, tyfkda, Rafael Ubal, Warren Toomey, Stephen Tu, Pablo Ventura,
Xi Wang, Keiichi Watanabe, Nicolas Wolovick, wxdao, Grant Wu, Jindong
Zhang, Icenowy Zheng, and Zou Chang Wei.

The code in the files that constitute xv6 is
Copyright 2006-2019 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu). The main purpose of xv6 is as a teaching
operating system for MIT's 6.828, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu. Once they are installed, and in your shell
search path, you can run "make qemu".
