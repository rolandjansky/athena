// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDVCollectionProxy.h 687822 2015-08-06 07:58:06Z krasznaa $
#ifndef XAODCORE_TDVCOLLECTIONPROXY_H
#define XAODCORE_TDVCOLLECTIONPROXY_H

// System include(s):
#include <cstddef>

// ROOT include(s):
#include <TGenCollectionProxy.h>

// Forward declaration(s):
class TClass;

namespace xAOD {

   /// @brief Dummy class to use as the @c DataVector template argument
   /// for the class we give to the Root proxy.
   class TDVCollectionProxyDummy {};

   /// A Root collection proxy for @c DataVector containers.
   ///
   /// In order for a container type to be treated properly by @c TTree::Draw
   /// and friends, Root must recognize that it is in fact a container type,
   /// and generate an appropriate proxy for it.  This doesn't happen
   /// automatically, though.  This class serves as an appropriate proxy
   /// for @c DataVector classes.  Like the Root proxy classes, it's based
   /// on making a hard cast to a dummy @c DataVector type.  But because
   /// @c DataVector's can inherit, we need to be careful about pointer
   /// conversions, something the Root proxy classes could ignore.
   /// We extend the Root environment buffer to cache information
   /// about the pointer offsets.
   ///
   /// @author Scott Snyder <Scott.Snyder@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 687822 $
   /// $Date: 2015-08-06 09:58:06 +0200 (Thu, 06 Aug 2015) $
   ///
   class TDVCollectionProxy
      : public TGenCollectionProxy {

   public:
      /// Make the definition of Sizing_t public
      typedef TGenCollectionProxy::Sizing_t Sizing_t;

      /// Constructor.
      /// @param conttype The name of the container type we're proxying.
      ///                 Note that this may be different from
      ///                 a @c DataVector of @a elttype for classes which
      ///                 derive from @c DataVector.
      TDVCollectionProxy( const char* conttype );
      /// Copy constructor.
      /// @param rhs The proxy to copy.
      TDVCollectionProxy( const TDVCollectionProxy& rhs );

      /// Start working with a new collection.
      /// @param objstart The address of the collection.
      virtual void PushProxy( void* objstart );

      /// Clone this object.
      virtual TVirtualCollectionProxy* Generate() const;

      /// Set the resize(...) function used by the proxy
      void SetResize( Sizing_t func );

   private:
      /// Function initialising the proxy just in time
      virtual TGenCollectionProxy* InitializeEx( Bool_t silent );
      /// Initialize the cached pointer offsets.
      void FindOffsets( const char* elttype,
                        const char* conttype );

      /// Name of the container that this class proxies
      TString fName;
      /// Flag for knowing whether the class was initialised already
      Bool_t fInitialized;

      /// The offset of the underlying @c DataVector in the declared type.
      ptrdiff_t fContoff;
      /// Offset between the DV base pointer and the full DV pointer
      ptrdiff_t fOffset;

      /// The element type to which the DV representation points.
      TClass* fEltBase;
      /// The declared element type of the DV.
      TClass* fEltType;

      // Disallow copying.
      TDVCollectionProxy& operator=( const TDVCollectionProxy& );

   }; // class TDVCollectionProxy

} // namespace xAOD

#endif // XAODCORE_TDVCOLLECTIONPROXY_H
