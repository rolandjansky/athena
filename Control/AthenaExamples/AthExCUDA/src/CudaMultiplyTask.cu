//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "CudaMultiplyTask.h"

// AthCUDA include(s).
#include "AthCUDAKernel/ArrayKernelTask.cuh"

namespace AthCUDAExamples {

   /// Structure implementing the GPU accelerated calculation
   struct CudaMultiplyTask {
      /// Function/operator performing the GPU accelerated calculation
      ATHCUDA_HOST_AND_DEVICE
      void operator()( std::size_t i, float* array, float multiplier ) {

         array[ i ] *= multiplier;
         return;
      }
   }; // struct CudaMultiplyTask

   /// Function setting up the task performing a simple linear transformation
   std::unique_ptr< AthCUDA::IKernelTask >
   make_CudaMultiplyTask( AthCUDA::KernelStatus& status,
                          std::vector< float >& array, float multiplier ) {

      return AthCUDA::make_ArrayKernelTask< CudaMultiplyTask >( status,
                                                                array.size(),
                                                                array.data(),
                                                                multiplier );
   }

} // namespace AthCUDAExamples
