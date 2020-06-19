// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDAINTERFACES_IKERNELTASK_H
#define ATHCUDAINTERFACES_IKERNELTASK_H

namespace AthCUDA {

   // Forward declaration(s).
   class StreamHolder;

   /// Abstract interface for setting up / running a CUDA task
   ///
   /// The expectation is that the implementation of such a task would
   /// either schedule itself to CUDA for execution, or if no GPU is
   /// available (at the time), would execute its calculation on the
   /// CPU instead.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class IKernelTask {

   public:
      /// Virtual destructor, to make vtable happy
      virtual ~IKernelTask() = default;

      /// Possible modes in which the kernel was executed
      enum KernelExecMode {
         /// The kernel was executed synchronously on the CPU
         Synchronous  = 0,
         /// The kernel was executed asynchronously on a CUDA device
         Asynchronous = 1
      };

      /// Execute the kernel using a specific stream
      ///
      /// If the stream is "invalid", the task needs to be executed in the
      /// current thread on the CPU instead.
      ///
      /// @param stream The stream to use in the task execution
      /// @return @c 0 if the call succeeded, something else otherwise
      ///
      virtual int execute( StreamHolder& stream ) = 0;

      /// Function called when an asynchronous execution finishes
      ///
      /// This function is set up to be called automatically when the task's
      /// execution is offloaded to a GPU, and that execution has finished.
      /// It can be used to notify some listener about the finish of the
      /// task.
      ///
      /// @param code A code representing the "successfulness" of the
      ///             asynchronous task
      /// @param mode The mode in which the kernel was executed
      /// @return @c 0 if the call succeeded, something else otherwise
      ///
      virtual int finished( int code, KernelExecMode mode ) = 0;

   }; // class IKernelTask

} // namespace AthCUDA

#endif // ATHCUDAINTERFACES_IKERNELTASK_H
