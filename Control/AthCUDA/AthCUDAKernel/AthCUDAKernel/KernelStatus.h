// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDAKERNEL_KERNELSTATUS_H
#define ATHCUDAKERNEL_KERNELSTATUS_H

// System include(s).
#include <mutex>

namespace AthCUDA {

   /// Helper object used for synchronising GPU kernel tasks
   ///
   /// It is meant to be used as a mixture of @c std::future and
   /// @c StatusCode. Client code can use it to wait for the (asynchronous)
   /// execution of a GPU calculation.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class KernelStatus {

   public:
      /// Default constructor
      KernelStatus();

      /// Function called by "the framework" when the CUDA kernel finished
      void finished( int code );

      /// Wait for the execution of the kernel to finish
      int wait();

   private:
      /// The result of the kernel execution
      int m_code;
      /// Mutex used for waiting for the CUDA task to finish
      std::mutex m_mutex;

   }; // class KernelStatus

} // namespace AthCUDA

#endif // ATHCUDAKERNEL_KERNELSTATUS_H
