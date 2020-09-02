// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDAINTERFACES_STREAMHOLDER_H
#define ATHCUDAINTERFACES_STREAMHOLDER_H

namespace AthCUDA {

   // Forward declaration(s).
   class IStreamPoolSvc;

   /// Helper class for handling CUDA streams
   ///
   /// This class is used to:
   ///   - be able to handle CUDA streams using pure C++ code;
   ///   - manage the ownership of CUDA streams during the job.
   ///
   /// Note that objects of this type can only be moved, they can not be
   /// copied.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class StreamHolder {

      /// Friend declaration(s).
#ifdef __CUDACC__
      friend cudaStream_t getStream( StreamHolder& );
#endif // __CUDACC__

   public:
      /// Default constructor
      StreamHolder();
      /// Constructor with the stream and the pool service
      StreamHolder( void* stream, IStreamPoolSvc* pool );
      /// Copy constructor
      StreamHolder( const StreamHolder& ) = delete;
      /// Move constructor
      StreamHolder( StreamHolder&& parent );
      /// Destructor
      ~StreamHolder();

      /// Copy assignment operator
      StreamHolder& operator=( const StreamHolder& ) = delete;
      /// Move assignment operator
      StreamHolder& operator=( StreamHolder&& rhs );

      /// Convenience operator for checking if the holder has a valid object
      operator bool() const;

   private:
      /// The managed stream
      void* m_stream;
      /// The pool that this holder is attached to
      IStreamPoolSvc* m_pool;

   }; // class StreamHolder

} // namespace AthCUDA

#endif // ATHCUDAINTERFACES_STREAMHOLDER_H
