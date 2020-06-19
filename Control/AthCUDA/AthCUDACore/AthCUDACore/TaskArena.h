// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDACORE_TASKARENA_H
#define ATHCUDACORE_TASKARENA_H

// TBB include(s).
#include <tbb/task_arena.h>

namespace AthCUDA {

   /// Function returning the TBB task arena that should be used for CUDA calls
   ///
   /// Since many (most?) CUDA calls use global locks, in order to avoid
   /// waiting times in the threads, one needs to schedule all CUDA operations
   /// using this single task arena, which takes care of executing all
   /// operations one by one. Using the TBB threads allocated for the rest of
   /// the job.
   ///
   /// @return A reference to the @c tbb::task_arena object to use for CUDA
   ///         calls.
   ///
   tbb::task_arena& taskArena();

} // namespace AthCUDA

#endif // ATHCUDACORE_TASKARENA_H
