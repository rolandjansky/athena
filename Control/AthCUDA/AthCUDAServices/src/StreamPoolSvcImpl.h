// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDASERVICES_STREAMPOOLSVCIMPL_H
#define ATHCUDASERVICES_STREAMPOOLSVCIMPL_H

// AthCUDA include(s).
#include "AthCUDAInterfaces/StreamHolder.h"

namespace AthCUDA {

   // Forward declaration(s).
   class IStreamPoolSvc;
   class StreamPoolSvcImplData;

   /// CUDA implementation of @c AthCUDA::StreamPoolSvc
   class StreamPoolSvcImpl {

   public:
      /// Constructor with all necessary arguments
      StreamPoolSvcImpl( unsigned int nStreams, IStreamPoolSvc& svc );
      /// Copy constructor
      StreamPoolSvcImpl( const StreamPoolSvcImpl& ) = delete;
      /// Move constructor
      StreamPoolSvcImpl( StreamPoolSvcImpl&& parent );
      /// Destructor
      ~StreamPoolSvcImpl();

      /// Copy assignment operator
      StreamPoolSvcImpl& operator=( const StreamPoolSvcImpl& ) = delete;
      /// Move assignment operator
      StreamPoolSvcImpl& operator=( StreamPoolSvcImpl&& rhs );

      /// @name Interface implementing parts of @c AthCUDA::IStreamPoolSvc
      /// @{

      /// Get a stream from the pool
      ///
      /// Note that the returned holder may point to a null memory address,
      /// in case no streams are available at the moment of the call.
      ///
      /// @return A stream holder describing a possibly abailable stream
      ///
      StreamHolder getAvailableStream();

      /// Put a given stream back into the pool
      ///
      /// This is a function called by @c AthCUDA::StreamHolder behind the
      /// scenes when it goes out of scope. It should not be called by hand.
      ///
      /// @param stream The stream holder whose stream is to be put back into
      ///               the pool
      ///
      void yieldStream( StreamHolder& stream );

      /// @}

   private:
      /// The service that this object is used by
      IStreamPoolSvc* m_svc;
      /// Data used by the class behind the scenes
      StreamPoolSvcImplData* m_data;

   }; // class StreamPoolSvcImpl

} // namespace AthCUDA

#endif // ATHCUDASERVICES_STREAMPOOLSVCIMPL_H
