// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDAINTERFACES_ISTREAMPOOLSVC_H
#define ATHCUDAINTERFACES_ISTREAMPOOLSVC_H

// Local include(s).
#include "AthCUDAInterfaces/StreamHolder.h"

// Gaudi include(s).
#include "GaudiKernel/IService.h"

namespace AthCUDA {

   /// Interface for a CUDA stream pool service
   ///
   /// Streams are provided to the entire job through this interface,
   /// allowing the code to balance memory copies and computations across
   /// all its configured streams.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class IStreamPoolSvc : public virtual IService {

      /// Make StreamHolder a friend of this interface
      friend class StreamHolder;

   public:
      /// Declare the interface ID
      DeclareInterfaceID( AthCUDA::IStreamPoolSvc, 1, 0 );

      /// Check whether any streams are available for the job
      ///
      /// This is a thread-safe way to check whether any CUDA streams are
      /// available at runtime.
      ///
      /// @return @c true if the service will under no circumstances return
      ///         a valid stream, @c false if it might
      ///
      virtual bool isEmpty() const = 0;

      /// Get a stream from the pool
      ///
      /// Note that the returned holder may point to a null memory address,
      /// in case no streams are available at the moment of the call.
      ///
      /// @return A stream holder describing a possibly abailable stream
      ///
      virtual StreamHolder getAvailableStream() = 0;

   protected:
      /// Put a given stream back into the pool
      ///
      /// This is a function called by @c AthCUDA::StreamHolder behind the
      /// scenes when it goes out of scope. It should not be called by hand.
      ///
      /// @param stream The stream holder whose stream is to be put back into
      ///               the pool
      virtual void yieldStream( StreamHolder& stream ) = 0;

   }; // class IStreamPoolSvc

} // namespace AthCUDA

#endif // ATHCUDAINTERFACES_ISTREAMPOOLSVC_H
