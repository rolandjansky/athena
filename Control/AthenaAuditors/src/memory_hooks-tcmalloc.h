/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// memory_hooks for tcmalloc which are used for the AthMemoryAuditor
// Rolf Seuster, Oct 2015

#ifndef ATHENAAUDITORS_MEMORYHOOKSTCMALLOC_H
#define ATHENAAUDITORS_MEMORYHOOKSTCMALLOC_H

#include <iostream>
#include <fstream>
#include <iomanip>

#include <cstdint>
#include <string.h>
#include <stdlib.h> // for getenv

// hooks and utilities from tcmalloc
#include <gperftools/malloc_extension.h>
#include <gperftools/malloc_hook.h>
#include <gperftools/stacktrace.h>
#include "gperftools/tcmalloc.h"

// various counters needed
static long long counter_tc_m(0);
static long long counter_tc_f(0);
static long long counter_tc_mtf(0);
static long long counter_tc_mm(0);
static long long counter_tc_mum(0);

/////////////////////////////////////////////////////////////////////
////////  tcmalloc start
/////////////////////////////////////////////////////////////////////

// define empty stubs, these will be interposed by the real tcmalloc
//  if they are actually called the job is badly misconfigured - should never happen
int MallocHook_RemoveNewHook(MallocHook_NewHook)
{ std::cerr << "TCMALLOC HOOKS: RemoveNewHook - Should not be here\n"; abort(); return 1; }
int MallocHook_RemoveDeleteHook(MallocHook_DeleteHook)
{ std::cerr << "TCMALLOC HOOKS: RemoveDeleteHook - Should not be here\n"; abort(); return 1; }
int MallocHook_AddNewHook(MallocHook_NewHook)
{ std::cerr << "TCMALLOC HOOKS: AddNewHook - Should not be here\n"; abort(); return 1; }
int MallocHook_AddDeleteHook(MallocHook_DeleteHook)
{ std::cerr << "TCMALLOC HOOKS: AddDeleteHook - Should not be here\n"; abort(); return 1; }
//  both are not really implemented yet, just counting - FIXME to be done
int MallocHook_AddMmapHook(MallocHook_MmapHook)
{ std::cerr << "TCMALLOC HOOKS: AddMmapHook - Should not be here\n"; abort(); return 1; }
int MallocHook_AddMunmapHook(MallocHook_MunmapHook)
{ std::cerr << "TCMALLOC HOOKS: AddMunmapHook - Should not be here\n"; abort(); return 1; }
// interpose this as well - we use this to discover if we have tcmalloc preloaded
MallocExtension* MallocExtension::instance() { AthMemoryAuditor::m_usetcmalloc=false; return nullptr; }

void athena_NewHook(const void* ptr, size_t size)
{
  counter_tc_m++;
  
  MallocHook_RemoveNewHook(&athena_NewHook);
  myBlocks_tc *b = new myBlocks_tc( (uintptr_t)ptr, size, curIndex, current_stage, stacktraceDepth );
  
  if(collectStacktraces)
    {
      b->allocatedFrom.resize(stacktraceDepth);
      unw_backtrace (b->allocatedFrom.data(), b->allocatedFrom.capacity());
    }
  std::pair<allocSet_tc::iterator, bool> i = allocset_tc.insert( *b );
  if(!i.second)
    {
      std::cerr << "ALREADY EXISTING " << std::hex << ptr << std::dec << "\n";
    }
  allocset_last=i.first;
  MallocHook_AddNewHook(&athena_NewHook);
}

void athena_DeleteHook(const void* ptr)
{
  counter_tc_f++;
  if(ptr==nullptr)
    {
      counter_tc_mtf++;
      return;
    }
  
  MallocHook_RemoveDeleteHook(&athena_DeleteHook);
  if ( allocset_last == allocset_tc.end() || allocset_last->allocated != (uintptr_t)ptr )
    {
      bg_tc->allocated=(uintptr_t)ptr;
      allocSet_tc::iterator i = allocset_tc.find( *bg_tc );
      if ( i != allocset_tc.end() )
	{
          myBlocks_tc* ptr = &*i;
	  allocset_tc.erase(i);
	  delete ptr;
	}
    }
  else
    {
      counter_fna++;
      //      std::cerr << "TCMALLOC delete-last\n";
      allocset_tc.erase(allocset_last);
      delete &(*allocset_last);
      allocset_last = allocset_tc.end();
    }
  MallocHook_AddDeleteHook(&athena_DeleteHook);
}

/* not fully implemented yet, just counting */
void athena_MmapHook(const void* /* result */,
		     const void* /* start */,
		     size_t /* size */,
		     int /* protection */,
		     int /* flags */,
		     int /* fd */,
		     off_t /* offset */)
{
  counter_tc_mm++;
  // std::cerr << "TCMALLOC mmap" << std::hex << result << "  " << size << std::dec << "\n";
}

void athena_MunmapHook(const void* /* ptr */, size_t /* size */)
{
  counter_tc_mum++;
  // std::cerr << "TCMALLOC munmap " << std::hex << ptr << "  " << size << std::dec << "\n";
}

void uninstall()
{
  MallocHook_RemoveNewHook(&athena_NewHook);
  MallocHook_RemoveDeleteHook(&athena_DeleteHook);
}

void my_init_tcmalloc()
{
  bg_tc = new myBlocks_tc();
  allocset_last=allocset_tc.end();
  
  if(MallocHook::AddNewHook(&athena_NewHook))
    std::cerr << "TCMALLOC NEW HOOK OK\n";
  if(MallocHook::AddDeleteHook(&athena_DeleteHook))
    std::cerr << "TCMALLOC DELETE HOOK OK\n";
  if(MallocHook::AddMmapHook(&athena_MmapHook))
    std::cerr << "TCMALLOC MMAP HOOK OK\n";
  if(MallocHook::AddMunmapHook(&athena_MunmapHook))
    std::cerr << "TCMALLOC MUNMAP HOOK OK\n";
  
  initialized=true;
}
#endif
