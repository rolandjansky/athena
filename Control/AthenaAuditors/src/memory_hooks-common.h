/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// common code for memory_hooks for tcmalloc and stdcmalloc which are used for the AthMemoryAuditor
// Rolf Seuster, Oct 2015

#ifndef ATHENAAUDITORS_MEMORYHOOKSCOMMON_H
#define ATHENAAUDITORS_MEMORYHOOKSCOMMON_H

#include <iostream>
#include <fstream>
#include <iomanip>

// needed for implementing hooks into tcmalloc
#include "gperftools/malloc_hook.h"

#include <cstdint>
#include <string.h>
#include <stdlib.h> // for getenv

#include <boost/intrusive/splay_set.hpp>

#define UNW_LOCAL_ONLY

#if defined(__clang__)
# pragma clang diagnostic push
# if __has_warning("-Wextern-c-compat")
#  pragma clang diagnostic ignored "-Wextern-c-compat"
# endif
#endif
#include "libunwind.h"
#if defined(__clang__)
# pragma clang diagnostic pop
#endif

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

static bool initialized(false);
static bool finished(false);

static uintptr_t context(0);
static uintptr_t contextFirst(0);
static uint32_t  current_stage(0);
static std::string defaultString = "N/A";

static size_t stacktraceDepth(25);

static bool collectStacktraces(true);

//using namespace boost::intrusive;

class myBlocks_tc : public boost::intrusive::bs_set_base_hook<boost::intrusive::optimize_size<false> >
  {
 public:
  uintptr_t          allocated;
  uintptr_t          allocsize;
  uint32_t           algindex;
  uint32_t           curstage;
  std::vector<void*> allocatedFrom;
  
 public:
  boost::intrusive::bs_set_member_hook<> member_hook_;
  
  myBlocks_tc()
    : allocated(0)
    , allocsize(0)
    , algindex(0)
    , curstage(0)
    { };
  
 myBlocks_tc(uintptr_t mem, uintptr_t s, uint32_t algi, uint32_t cstage, size_t stDepth)
   : allocated(mem)
    , allocsize(s)
    , algindex(algi)
    , curstage(cstage)
    , allocatedFrom(stDepth,nullptr)  { };
  
  friend bool operator<  ( const myBlocks_tc &a, const myBlocks_tc &b )
  {
    return a.allocated < b.allocated;
  };
  
  friend bool operator>  ( const myBlocks_tc &a, const myBlocks_tc &b )
  {
    return a.allocated > b.allocated;
  };
  
  friend bool operator== ( const myBlocks_tc &a, const myBlocks_tc &b)
  {
    return a.allocated == b.allocated;
  };
};

myBlocks_tc *bg_tc;

typedef boost::intrusive::splay_set< myBlocks_tc, boost::intrusive::base_hook<boost::intrusive::bs_set_base_hook<> > > allocSet_tc;

static allocSet_tc::iterator allocset_last;
static allocSet_tc allocset_tc;
static allocSet_tc free_set_tc;

#endif
