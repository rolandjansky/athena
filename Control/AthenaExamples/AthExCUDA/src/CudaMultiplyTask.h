// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXCUDA_CUDAMULTIPLYTASK_H
#define ATHEXCUDA_CUDAMULTIPLYTASK_H

// AthCUDA include(s).
#include "AthCUDAInterfaces/IKernelTask.h"

// System include(s).
#include <memory>
#include <vector>

// Forward declaration(s).
namespace AthCUDA {
   class KernelStatus;
}

namespace AthCUDAExamples {

   /// Function setting up the task performing a simple linear transformation
   std::unique_ptr< AthCUDA::IKernelTask >
   make_CudaMultiplyTask( AthCUDA::KernelStatus& status,
                          std::vector< float >& array, float multiplier );

} // namespace AthCUDAExamples

#endif // ATHEXCUDA_CUDAMULTIPLYTASK_H
