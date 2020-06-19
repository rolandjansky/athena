// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDACore/StreamHolderHelpers.cuh"

// Project include(s).
#include "AthCUDAInterfaces/StreamHolder.h"

namespace AthCUDA {

   cudaStream_t getStream( StreamHolder&
#ifdef __CUDACC__
                                         holder
#endif // __CUDACC__
                                                ) {

#ifdef __CUDACC__
      return reinterpret_cast< cudaStream_t >( holder.m_stream );
#else
      return nullptr;
#endif // __CUDACC__
   }

} // namespace AthCUDA
