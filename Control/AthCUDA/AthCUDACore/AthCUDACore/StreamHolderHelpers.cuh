// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDACORE_STREAMHOLDERHELPERS_CUH
#define ATHCUDACORE_STREAMHOLDERHELPERS_CUH

// Local include(s).
#include "AthCUDACore/Macros.cuh"

namespace AthCUDA {

   // Forward declaration(s).
   class StreamHolder;

   /// Get the stream held by a helper object
   cudaStream_t getStream( StreamHolder& holder );

} // namespace AthCUDA

#endif // ATHCUDACORE_STREAMHOLDERHELPERS_CUH
