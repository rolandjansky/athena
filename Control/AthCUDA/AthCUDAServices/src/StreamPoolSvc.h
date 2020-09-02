// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDASERVICES_STREAMPOOLSVC_H
#define ATHCUDASERVICES_STREAMPOOLSVC_H

// Local include(s).
#include "StreamPoolSvcImpl.h"

// AthCUDA include(s).
#include "AthCUDAInterfaces/IStreamPoolSvc.h"

// Framework include(s).
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthService.h"

// System include(s).
#include <memory>

namespace AthCUDA {

   /// Service managing a fixed number of CUDA streams for an application
   ///
   /// The service provides streams to the callers by endlessly looping over
   /// the available queue of streams that it allocates during the
   /// initialisation.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class StreamPoolSvc : public extends< AthService, IStreamPoolSvc > {

   public:
      /// Inherit the base class's constructor(s)
      using extends::extends;

      /// @name Interface inherited from @c IService
      /// @{

      /// Initialise the service
      virtual StatusCode initialize() override;

      /// Finalise the service
      virtual StatusCode finalize() override;

      /// @}

      /// @name Interface inherited from @c AthCUDA::IStreamPoolSvc
      /// @{

      /// Check whether any streams are available for the job
      ///
      /// This is a thread-safe way to check whether any CUDA streams are
      /// available at runtime.
      ///
      /// @return @c true if the service will under no circumstances return
      ///         a valid stream, @c false if it might
      ///
      virtual bool isEmpty() const override;

      /// Get a stream from the pool
      ///
      /// Note that the returned holder may point to a null memory address,
      /// in case no streams are available at the moment of the call.
      ///
      /// @return A stream holder describing a possibly abailable stream
      ///
      virtual StreamHolder getAvailableStream() override;

      /// Put a given stream back into the pool
      ///
      /// This is a function called by @c AthCUDA::StreamHolder behind the
      /// scenes when it goes out of scope. It should not be called by hand.
      ///
      /// @param stream The stream holder whose stream is to be put back into
      ///               the pool
      virtual void yieldStream( StreamHolder& stream ) override;

      /// @}

   private:
      /// @name Service properties
      /// @{

      /// The number of streams to use
      Gaudi::Property< unsigned int > m_nStreams{ this, "NStreams", 2,
         "The number of CUDA streams to use" };

      /// @}

      /// @name Implementation variable(s)
      /// @{

      /// The object implementing the interaction with CUDA
      std::unique_ptr< StreamPoolSvcImpl > m_impl;

      /// @}

   }; // class StreamPoolSvc

} // namespace AthCUDA

#endif // ATHCUDASERVICES_STREAMPOOLSVC_H
