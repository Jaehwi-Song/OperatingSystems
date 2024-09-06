// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  int len;  // for counting # of free pages
} kmem;

// array for reference count of physical addr (PHYSTOP is defined in memlayout.h, PG_BIT in mmu.h)
uint ref_count[PHYSTOP >> PG_BIT];

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  // initialize # of free pages as 0
  kmem.len = 0;
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
  { 
    // initialize ref_count to 0 
    ref_count[V2P(p) >> PG_BIT] = 0;
    kfree(p);
    // calculate # of free pages
    kmem.len++;
  }
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");
  
  if(kmem.use_lock)
    acquire(&kmem.lock);
  // decrement ref count of V2P(v)
  if (get_refc(V2P(v)) > 0) {
    decr_refc(V2P(v));
  }  
  // if this is the last reference for a certain physical page
  if(get_refc(V2P(v)) == 0) {  
    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);
    r = (struct run*)v;
    r->next = kmem.freelist;
    kmem.freelist = r;
    // increment free page num by 1
    kmem.len++;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    // decrement free page num by 1
    kmem.len--;
    // set ref_count to 1 when first allocated
    ref_count[V2P((char*)r)  >> PG_BIT] = 1;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  
  return (char*)r;
}

// system call for int countfp(void)
int
countfp(void)
{
  return kmem.len;
}

// increment ref_count of a certain physical page
void 
incr_refc(uint pa) 
{
  ref_count[pa >> PG_BIT]++;
}

// decrement ref_count of a certain physical page
void
decr_refc(uint pa)
{
  ref_count[pa >> PG_BIT]--;
}

// return current ref_count of a certain physical page
int
get_refc(uint pa)
{
  uint count = ref_count[pa >> PG_BIT];
  return (int)count;
}
