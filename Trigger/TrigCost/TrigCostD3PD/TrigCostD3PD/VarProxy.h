// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 539451 2013-03-01 08:33:45Z krasznaa $
#ifndef D3PDREADER_VARPROXY_H
#define D3PDREADER_VARPROXY_H

// STL include(s):
#include <vector>

// Local include(s):
#include "VarHandle.h"

namespace D3PDReader {

   /**
    *  @short Base class for the VarProxy versions
    *
    *         This class is needed for technical reasons in order to handle
    *         user defined variables in a nice way.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 539451 $
    * $Date: 2013-03-01 09:33:45 +0100 (Fri, 01 Mar 2013) $
    */
   class VarProxyBase {

   public:
      /// Destructor, to make vtable happy
      virtual ~VarProxyBase() {}

      /// Check if the variable is available in the input
      virtual ::Bool_t IsAvailable() const = 0;

   }; // class VarProxyBase

   /**
    *  @short Class acting as a proxy to one element of an std::vector
    *
    *         This class is used in the "Element" classes to represent one
    *         object from a collection.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 539451 $
    * $Date: 2013-03-01 09:33:45 +0100 (Fri, 01 Mar 2013) $
    */
   template< typename Type >
   class VarProxy : public VarProxyBase {

   public:
      /// Convenience typedef of the used variable
      typedef Type& result_type;
      /// Convenience typedef of the used variable (const version)
      typedef const Type& const_result_type;

      /// Constructor specifying the handle object this proxy uses
      VarProxy( const VarHandle< std::vector< Type >* >& handle, size_t i );
      /// Copy constructor
      VarProxy( const VarProxy< Type >& parent );
      /// Copy operator
      VarProxy< Type >& operator=( const VarProxy< Type >& parent );

      /// Check if the variable is available in the input
      virtual ::Bool_t IsAvailable() const;

      /// Operator returning the variable belonging to this object
      result_type operator()();
      /// Operator returning the variable belonging to this object (constant version)
      const_result_type operator()() const;

   private:
      VarHandle< std::vector< Type >* >* fHandle; ///< Pointer to the handle object
      size_t fIndex; ///< Index inside the handle object's vector
      Type fDummy; ///< Object returned by default

   }; // class VarProxy

} // namespace D3PDReader

// Include the implementation:
#ifndef __CINT__
#include "VarProxy.icc"
#endif // __CINT__

#endif // D3PDREADER_VARPROXY_H
