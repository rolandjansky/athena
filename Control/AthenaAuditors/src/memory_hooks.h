/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// memory_hooks which are used for the AthMemoryAuditor
// Rolf Seuster, August 2015

#include <iostream>
#include <fstream>
#include <iomanip>

#include <map>

/* for Prototypes for __malloc_hook, __free_hook */
#include <malloc.h>

#include <cstdint>
#include <string.h>
#include <stdlib.h> // for getenv

// needed for placement new
#include <new> 

static std::string name;

struct aiStruct
{
  aiStruct() : index(0), stacktrace(0) {};
  aiStruct(uint16_t i, uint16_t s) : index(i), stacktrace(s) {};
  uint16_t index;
  uint16_t stacktrace;

};

static std::map<std::string, aiStruct> arrayAlgIndex;
static std::map<uint32_t, std::string> algorithms;

static uint32_t curMaxIndex(0);
static uint32_t curIndex(0);

//#define MYDEBUG
#undef MYDEBUG

#ifdef  MYDEBUG
static void printAllocation( void*ptr, size_t n );
#endif // MYDEBUG

/*  structure of redzones:
    bytes     0- 7: pointer to next
    bytes     8-15: pointer to prev
    bytes    16-17: delta to payload
    bytes    18-23: size of payload
    bytes    24-31: ascii " redzone"
    
    payload -22-23: delta to payload
    payload -16-21: size of payload
    payload - 7 -0: ascii " redzone"
    // last three might be same as above, will be different for 'memalign' with alignment > 32
    
    payload
    
    payload   +0-7: ascii "enozder "
    payload   8-15: size of payload    
    payload +16-19: stage
    payload +20-23: index to algorithm name
 */

// deltaLow must be multiple of 16 - Eigen / AV requires this alignment
const unsigned int deltaLow=32;
const unsigned int deltaHigh=32;

class node
{
public:
  node() : m_next(0), m_prev(0), m_deltaPayload(32)
  {
    m_canary[0]='r'; m_canary[1]='e'; m_canary[2]='d'; m_canary[3]='z';
    m_canary[4]='o'; m_canary[5]='n'; m_canary[6]='e'; m_canary[7]=' ';
  };
  node(node* n, node* p, uint16_t d=32) : m_next(n), m_prev(p),  m_deltaPayload(d)
  {
    m_canary[0]='r'; m_canary[1]='e'; m_canary[2]='d'; m_canary[3]='z';
    m_canary[4]='o'; m_canary[5]='n'; m_canary[6]='e'; m_canary[7]=' ';
  };
  node(bool b)
  {
    if(b)
      {
	m_next=0;
	m_prev=0;
	m_deltaPayload=32;
	m_canary[0]='r'; m_canary[1]='e'; m_canary[2]='d'; m_canary[3]='z';
	m_canary[4]='o'; m_canary[5]='n'; m_canary[6]='e'; m_canary[7]=' ';
      }
  };
  void setNext(node* n) { m_next=n; };
  void setPrev(node* p) { m_prev=p; };
  node* getNext() { return m_next; };
  node* getPrev() { return m_prev; };
  void setSize(size_t s) { m_sizeLow=(uint32_t)(s&0xFFFFFFFF); m_sizeHigh=(s>>32); };
  size_t getSize() { return (size_t)m_sizeLow+(((size_t)m_sizeHigh)<<32); };
  void fillRedZone()
  {
    m_canary[0]='r'; m_canary[1]='e'; m_canary[2]='d'; m_canary[3]='z';
    m_canary[4]='o'; m_canary[5]='n'; m_canary[6]='e'; m_canary[7]=' ';
  };  
  
private:
  node*  m_next;
  node*  m_prev;
  uint16_t m_deltaPayload;
  uint16_t m_sizeHigh;
  uint32_t m_sizeLow;
  char      m_canary[8];
};

class redzone
{
public:
  redzone() : m_s(0), m_stage(0), m_algIndex(0)
  {
    m_canary[0] ='e'; m_canary[1] ='n'; m_canary[2] ='o'; m_canary[3] ='z';
    m_canary[4] ='d'; m_canary[5] ='e'; m_canary[6] ='r'; m_canary[7] =' ';
    m_canary[8] ='r'; m_canary[9] ='e'; m_canary[10]='d'; m_canary[11]='z';
    m_canary[12]='o'; m_canary[13]='n'; m_canary[14]='e'; m_canary[15]=' ';
  };
  redzone(bool b)
  {
    if(b)
      {
	m_s=0;
	m_stage=0;
	m_algIndex=0;
	m_canary[0] ='e'; m_canary[1] ='n'; m_canary[2] ='o'; m_canary[3] ='z';
	m_canary[4] ='d'; m_canary[5] ='e'; m_canary[6] ='r'; m_canary[7] =' ';
	m_canary[8] ='r'; m_canary[9] ='e'; m_canary[10]='d'; m_canary[11]='z';
	m_canary[12]='o'; m_canary[13]='n'; m_canary[14]='e'; m_canary[15]=' ';
      }
  };
  redzone(size_t s, uint32_t st, uint32_t ai) : m_s(s), m_stage(st), m_algIndex(ai)
  {
    m_canary[0] ='e'; m_canary[1] ='n'; m_canary[2] ='o'; m_canary[3] ='z';
    m_canary[4] ='d'; m_canary[5] ='e'; m_canary[6] ='r'; m_canary[7] =' ';
    m_canary[8] ='R'; m_canary[9] ='E'; m_canary[10]='D'; m_canary[11]='-';
    m_canary[12]='Z'; m_canary[13]='O'; m_canary[14]='N'; m_canary[15]='E';
  };
  size_t   getSize() { return m_s; };
  uint32_t getStage() { return m_stage; };
  void clearStage() { m_stage=0; };
  uint32_t getAlgIndex() { return m_algIndex; };
  
private:
  char      m_canary[16];
  size_t    m_s;
  uint32_t  m_stage;
  uint32_t  m_algIndex;
};

static node s_first;
static node s_last;

static bool initialized(false);

static uintptr_t s_sbrk_ptr(0);

/* Prototypes for our hooks.  */
static void  my_init_hook (void);
static void *my_malloc_hook (size_t, const void *);
static void *my_realloc_hook (void *, size_t, const void *);
static void *my_memalign_hook (size_t, size_t, const void *);
static void  my_free_hook (void*, const void *);

static void *(*old_malloc_hook)(size_t, const void *);
static void *(*old_realloc_hook)(void *, size_t, const void *);
static void *(*old_memalign_hook)(size_t, size_t, const void *);
static void  (*old_free_hook)(void*, const void *);
     
// various counters needed
static long long counter_m(0);
static long long counter_f(0);
static long long counter_fna(0);
static long long counter_r(0);
static long long counter_rs(0);
static long long counter_rg(0);
static long long counter_rm(0);
static long long counter_rf(0);
static long long counter_ma(0);

static uintptr_t context(0);
static uintptr_t contextFirst(0);
static uint32_t  current_stage(0);
static std::string defaultString = "N/A";

static size_t stacktraceDepth(25);

static bool collectStacktraces(true);

#include <boost/intrusive/avl_set.hpp>

#define UNW_LOCAL_ONLY
#include "libunwind.h"

using namespace boost::intrusive;

class myBlocks : public avl_set_base_hook<optimize_size<true> >
{
 public:
  uintptr_t          allocated;
  std::vector<void*> allocatedFrom;
  
 public:
  //This is a member hook
  avl_set_member_hook<> member_hook_;
  
  myBlocks()
    : allocated(0)
    { };
  
 myBlocks(uintptr_t mem, size_t stDepth)
    : allocated(mem)
    , allocatedFrom(stDepth,nullptr)  { };
  
  friend bool operator<  ( const myBlocks &a, const myBlocks &b )
  {
    return a.allocated < b.allocated;
  };
  
  friend bool operator>  ( const myBlocks &a, const myBlocks &b )
  {
    return a.allocated > b.allocated;
  };
  
  friend bool operator== ( const myBlocks &a, const myBlocks &b)
  {
    return a.allocated == b.allocated;
  }
};

myBlocks *bg;

typedef avl_set< myBlocks, compare<std::greater<myBlocks> > >  allocSet;
static allocSet allocset;
static allocSet free_set;

static void *my_malloc_hook (size_t size, const void* /* caller */)
{
  uintptr_t result(0);
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  __memalign_hook = old_memalign_hook;
  __free_hook = old_free_hook;
  /* Call recursively */
  
  // if result is in region w/o redzones, try allocating again
  do
    {
      result = (uintptr_t) malloc (size+deltaLow+deltaHigh);
    } while ( result < s_sbrk_ptr );
  
  node* n = new ((void*)result) node( s_first.getNext(), &s_first );
  s_first.getNext()->setPrev(n);
  s_first.setNext(n);
  n->setSize(size);
  
  counter_m++;
  
  // placement new of redzone, compiler complains about r unused ...
  __attribute__ ((unused)) redzone *r = new ((void*)(result+deltaLow+size)) redzone( size, current_stage, curIndex );
  
  if(collectStacktraces)
    {
      myBlocks *b = new myBlocks( (uintptr_t)result, stacktraceDepth );
      unw_backtrace (b->allocatedFrom.data(), b->allocatedFrom.capacity());
      allocSet::iterator i = allocset.find( *b );
      if ( i == allocset.end() )
	{
	  allocset.insert( *b );
	}
    }
  
  result+=deltaLow;
  
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_realloc_hook = __realloc_hook;
  old_memalign_hook = __memalign_hook;
  old_free_hook = __free_hook;
  
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __realloc_hook = my_realloc_hook;
  __memalign_hook = my_memalign_hook;
  __free_hook = my_free_hook;
  return (void*)result;
}

static void
my_free_hook (void *ptr, const void* /* caller */)
{
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  __memalign_hook = old_memalign_hook;
  __free_hook = old_free_hook;
  
  /* Call recursively */
  // only free if allocation was done with redzone, i.e. above m_sbtr_ptr
  // using uintptr_t to silence compiler warnings
  uintptr_t p=(uintptr_t)ptr;
  if (p > s_sbrk_ptr + deltaLow )
    {
      // need to protect against system calls returning ointers intto kernel memory
      if(p<0x7f0000000000)
	{
	  p-=deltaLow;
	  
	  node* n=new ((void*)p) node(false);
	  // std::cerr << "free: " << p << " " << n << "\n";
	  node* nn=n->getNext();
	  node* np=n->getPrev();
	  
	  nn->setPrev(np);
	  np->setNext(nn);
	}
      free ((void*)p);
      
      // if(collectStacktraces)
      // {
      bg->allocated=(uintptr_t)p;
      allocSet::iterator i = allocset.find( *bg );
      if ( i != allocset.end() )
	{
	  allocset.erase(i);
	  delete &(*i);
	  // don't delete yet - here or later in [1]
	  // BG delete b;
	}
      else
	{
	  counter_fna++;
	}
      // }
      
      p+=deltaLow;
    }
  
  // size_t size_st;
  // size_st = mybacktrace (array, BTSIZE);
  
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_realloc_hook = __realloc_hook;
  old_memalign_hook = __memalign_hook;
  old_free_hook = __free_hook;
  
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __realloc_hook = my_realloc_hook;
  __memalign_hook = my_memalign_hook;
  __free_hook = my_free_hook;
  counter_f++;
}

static void *
my_realloc_hook(void *ptr, size_t size, const void * /* caller */)
{
  uintptr_t result(0);
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  __memalign_hook = old_memalign_hook;
  __free_hook = old_free_hook;
  
  /* Call recursively */
  size_t oldsize(0);
  size_t sizeNoRedZones(size);
  node* n(0);
  node* nn(0);
  node* np(0);
  
  // using uintptr_t to silence compiler warnings
  uintptr_t p=(uintptr_t)ptr;
  // did initial allocation had redzones ?
  if (p > s_sbrk_ptr+deltaLow )
    {
      p-=deltaLow;
      // then we also know the size of the old allocation
      n=new ((void*)p) node(false);
      oldsize=n->getSize();
      nn=n->getNext();
      np=n->getPrev();
    }
  
  // size==0 means free'ing memory
  if(size)
    size+=deltaLow+deltaHigh;
  else
    counter_rf++;
  
  // distinguish the case when used as free, modify linked list before deallocation
  if (size == 0 && p > s_sbrk_ptr )
    {
      node* nf=new ((void*)p) node(false);
      node* nn=nf->getNext();
      node* np=nf->getPrev();
      nn->setPrev(np);
      np->setNext(nn);
      
      // remove from list
      bg->allocated=(uintptr_t)p;
      allocSet::iterator i = allocset.find( *bg );
      if ( i != allocset.end() )
	{
	  allocset.erase(i);
	  delete &(*i);
	}
    }
  
  // call real realloc
  result = (uintptr_t)realloc((void*)p, size);
  
  // if new, real allocation is in region w/o redzones, try again via malloc, copy memory content for each try
  if(size>0)
    while ( result < s_sbrk_ptr )
      {
	void* r_new(0);
	r_new = malloc(size);
	memcpy(r_new,(void*)result,size);
	result = (uintptr_t)r_new;
      }
  
  // allocate in freshly deallocated memory below s_sbrk_ptr in case of free
  if ( size == 0 )
    {
      if ( p < s_sbrk_ptr )
	{
	  uintptr_t pp(0);
	  while ( pp < s_sbrk_ptr )
	    {
	      pp=(uintptr_t)malloc(sizeof(void*));
	    }
	}
    }
  else
    {
      char* start((char*)result);
      
      // constructor with placement new, no overwriting of memory (parameter to construtor is false)
      n = new ((void*)result) node(false);
      
      // test, if we are dealing with a real allocation in redzone land
      if ( ( result > s_sbrk_ptr ) && ( size > deltaLow+deltaHigh ) )
	{
	  // same pointer ? Then just size was increased / decreased
	  if ( p == result )
	    {
	      // adjust new size
	      n->setSize(sizeNoRedZones);
	      
	      // put redzone at new end of allocation, silence compiler about unused variable
	      __attribute__ ((unused)) redzone *r = new ((void*)(result+deltaLow+sizeNoRedZones)) redzone( sizeNoRedZones, current_stage, curIndex );
	      if ( oldsize < sizeNoRedZones )
		{
		  // fill extended memory with dummy value, conditons guarantee alloction is and was with redzones
		  memset((void*)(result+deltaLow+oldsize),0xaa,sizeNoRedZones-oldsize);
		  counter_rg++;
		}
	      else
		counter_rs++;
	    }
	  else
	    {
	      // possibly, we need to move some memory around, if it was allocated w/o redzone,
	      //  but realloc moved it into redzone area...
	      if ( p && ( p < s_sbrk_ptr ) && ( result > s_sbrk_ptr ) && ( size > deltaLow+deltaHigh ) )
		{
		  for( int s(size-1-deltaLow-deltaHigh); s>=0; --s )
		    {
		      start[deltaLow+s]=start[s];
		    }
		  
		  n = new ((void*)result) node( s_first.getNext(), &s_first );
		  s_first.getNext()->setPrev(n);
		  s_first.setNext(n);
		  
		  // we free'd stuff, so let's allocate it again - for now it is leaking
		  uintptr_t pp(0);
		  while ( pp < s_sbrk_ptr )
		    {
		      pp=(uintptr_t)malloc(sizeof(void*));
		    }
		}
	      // different pointer, need to overwrite some values...
	      if ( p && ( p > s_sbrk_ptr ) && ( result > s_sbrk_ptr ) && (p != result ) && ( size > deltaLow+deltaHigh ) )
		{
		  n = new ((void*)result) node( nn, np );
		  nn->setPrev(n);
		  np->setNext(n);
		  
		  // size and redzone done later
		  if ( oldsize < sizeNoRedZones )
		    {
		      memset((void*)(result+deltaLow+oldsize),0xaa,sizeNoRedZones-oldsize);
		    }
		}
	      // initial pointer was zero, so new allocation...
	      if ( p == 0 && ( result > s_sbrk_ptr ) && ( size > deltaLow+deltaHigh ) )
		{
		  node* n = new ((void*)result) node( s_first.getNext(), &s_first );
		  s_first.getNext()->setPrev(n);
		  s_first.setNext(n);
		  
		  memset((void*)(result+deltaLow),0x55,sizeNoRedZones);
		  
		  counter_rm++;
		}		  
	      
	      // keep stacktrace for all three cases above
	      if(collectStacktraces)
		{
		  myBlocks *b = new myBlocks( (uintptr_t)result, stacktraceDepth );
		  unw_backtrace (b->allocatedFrom.data(), b->allocatedFrom.capacity());
		  allocSet::iterator i = allocset.find( *b );
		  if ( i == allocset.end() )
		    {
		      allocset.insert( *b );
		    }
		}
	      
	      //required for both
	      n->setSize(sizeNoRedZones);
	      
	      // placement new of redzone, compiler complains about r unused ...
	      __attribute__ ((unused)) redzone *r = new ((void*)(result+deltaLow+sizeNoRedZones)) redzone( sizeNoRedZones, current_stage, curIndex );
	    }
	}
    }
  if (result > s_sbrk_ptr )
    result+=deltaLow;
  if (p > s_sbrk_ptr )
    p+=deltaLow;
  
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_realloc_hook = __realloc_hook;
  old_memalign_hook = __memalign_hook;
  old_free_hook = __free_hook;
  
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __realloc_hook = my_realloc_hook;
  __memalign_hook = my_memalign_hook;
  __free_hook = my_free_hook;
  counter_r++;
  return (void*)result;
}

static void *
my_memalign_hook (size_t alignment, size_t size, const void * /* caller */ )
{
  uintptr_t result;
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  __memalign_hook = old_memalign_hook;
  __free_hook = old_free_hook;
  
  /* Call recursively */
  size+=deltaLow+deltaHigh+alignment;
  result = (uintptr_t)memalign(alignment, size);
  
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_realloc_hook = __realloc_hook;
  old_memalign_hook = __memalign_hook;
  old_free_hook = __free_hook;
  
  node* n = new ((void*)result) node( s_first.getNext(), &s_first );
  s_first.getNext()->setPrev(n);
  s_first.setNext(n);
  
  result+=alignment;
  
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __realloc_hook = my_realloc_hook;
  __memalign_hook = my_memalign_hook;
  __free_hook = my_free_hook;
  counter_ma++;
  return (void*)result;
}

#ifdef MYDEBUG

static int s_level;

static
void 
my_stop ()
{
  s_level--;
  std::cerr << "STOP " << s_level << "\n";
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  __memalign_hook = old_memalign_hook;
  __free_hook = old_free_hook;
  
  old_malloc_hook = __malloc_hook;
  old_realloc_hook = __realloc_hook;
  old_memalign_hook = __memalign_hook;
  old_free_hook = __free_hook;
}

static
void 
my_start ()
{
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __realloc_hook = my_realloc_hook;
  __memalign_hook = my_memalign_hook;
  __free_hook = my_free_hook;

  s_level++;
  std::cerr << "START " << s_level << "\n";
}

static
void printAllocation( void*ptr, size_t n )
{
  bool hasRedzone=false;
  unsigned char *cptr=(unsigned char*)ptr;
  int nread;
  unsigned char buf[16];
  
  std::cerr << "Printing information about allocation at: " << ptr << " of size " << n << "\n";
  if((uintptr_t)ptr>s_sbrk_ptr)
    {
      hasRedzone=true;
      std::cerr << " allocation has redzone " << "\n";
    }
  if( hasRedzone )
    {
      std::cerr << "Redzone before" << "\n";
      std::cerr << std::hex << std::setfill('0');
      for( int l(0); l<32; l+=16 )
	{
	  for( nread = 0; nread < 16; nread++ )
	    buf[nread]= *(cptr-32+l+nread);
	  
	  // Show the hex codes
	  for( int i = 0; i < 16; i++ )
	    {
	      if( i % 8 == 0 ) std::cerr << ' ';
	      if( i < nread )
		std::cerr << ' ' << std::setw(2) << (unsigned int)buf[i];
	      else 
		std::cerr << "   ";
	    }
	  
	  // Show printable characters
	  std::cerr << "  >";
	  for( int i = 0; i < nread; i++)
	    {
	      if( buf[i] < 32 || buf[i] > 128 ) std::cerr << '.';
	      else std::cerr << buf[i];
	    }
	  
	  std::cerr << "<\n";
	}
    }
  
  std::cerr << "Payload:\n";
  std::cerr << std::hex << std::setfill('0');
  for( unsigned int l(0); l<n; l+=16 )
    {
      for( nread = 0; nread < 16; nread++ )
	buf[nread]= *(cptr+l+nread);
      
      // Show the hex codes
      for( int i = 0; i < 16; i++ )
	{
	  if( i % 8 == 0 ) std::cerr << ' ';
	  if( i < nread && l+i<n )
	    std::cerr << ' ' << std::setw(2) << (unsigned)buf[i];
	  else 
	    std::cerr << "   ";
	}
      
      // Show printable characters
      std::cerr << "  >";
      for( int i = 0; i < nread; i++)
	{
	  if( buf[i] < 32 || buf[i] > 128 ) std::cerr << '.';
	  else std::cerr << buf[i];
	}
      
      std::cerr << "<\n";
    }
  
  if( hasRedzone )
    {
      std::cerr << "Redzone after" << "\n";
      std::cerr << std::hex << std::setfill('0');
      for( int l(0); l<32; l+=16 )
	{
	  for( nread = 0; nread < 16; nread++ )
	    buf[nread]= *(cptr+n+l+nread);
	  
	  // Show the hex codes
	  for( int i = 0; i < 16; i++ )
	    {
	      if( i % 8 == 0 ) std::cerr << ' ';
	      if( i < nread )
		std::cerr << ' ' << std::setw(2) << (unsigned int)buf[i];
	      else 
		std::cerr << "   ";
	    }
	  
	  // Show printable characters
	  std::cerr << "  >";
	  for( int i = 0; i < nread; i++)
	    {
	      if( buf[i] < 32 || buf[i] > 128 ) std::cerr << '.';
	      else std::cerr << buf[i];
	    }
	  
	  std::cerr << "<\n";
	}
    }
  std::cerr << std::dec << "\n";
}

static void my_test()
{
  void *p0=malloc(20);
  memset(p0,0x1,20); 
  
  my_stop();
  printAllocation(p0,20);
  my_start();
  
  void *p1=malloc(32);
  memset(p1,0x2,32);
  void *pd=malloc(32);
  memset(pd,0x7,32);
  
  my_stop();
  printAllocation(p1,32);
  my_start();
  
  void *p2=realloc(p1, 48);
  my_stop();
  std::cerr << "realloc : " << p2 << " " << p1 << "\n";
  printAllocation(p2,48);
  my_start();
  
  p1=realloc(p2, 32);
  my_stop();
  std::cerr << "realloc : " << p2 << " " << p1 << "\n";
  printAllocation(p1,32);
  my_start();
  
  p2=realloc(p0, 48);
  my_stop();
  std::cerr << "realloc : " << p2 << " " << p0 << "\n";
  printAllocation(p2,48);
  my_start();
  
  free(p2);
  my_stop();
  std::cerr << "free :\n";
  printAllocation(pd,32);
  my_start();
}
#endif // MYDEBUG

static void
my_init_hook (void)
{
  // for glibc's malloc called stdcmalloc for athena.py
  //  disable using mmap for large allocations
  mallopt(M_MMAP_MAX, 0);
  // disable fast bins
  mallopt(M_MXFAST, 0);
  
  s_sbrk_ptr=(uintptr_t)sbrk(0);
  uintptr_t p(0);
  
  while ( p < s_sbrk_ptr )
    {
      p=(uintptr_t)malloc(sizeof(void*));
    }
  
#ifdef MYDEBUG
  std::cerr << "INIT : " << s_sbrk_ptr << "\n";
  std::cerr << "INIT : redzone before / after " << deltaLow << " / " << deltaHigh << " bytes\n";
  
  std::cerr << "INIT : sizes " << sizeof(node) << " / " << sizeof(redzone) << "\n";
  
  std::cerr << "INIT : s_first / s_last " << &s_first << " / " << &s_last << "\n";
#endif // MYDEBUG
  
  s_first.setNext(&s_last);
  s_last.setPrev(&s_first);
  
  bg = new myBlocks();
  
  // save underying old hooks
  old_malloc_hook = __malloc_hook;
  old_realloc_hook = __realloc_hook;
  old_memalign_hook = __memalign_hook;
  old_free_hook = __free_hook;
  // set new hooks
  __malloc_hook = my_malloc_hook;
  __realloc_hook = my_realloc_hook;
  __memalign_hook = my_memalign_hook;
  __free_hook = my_free_hook;
  initialized=true;
  
#ifdef MYDEBUG
  s_level=1;
#endif // MYDEBUG
}
