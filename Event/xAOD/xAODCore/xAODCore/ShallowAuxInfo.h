// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowAuxInfo.h 611401 2014-08-12 12:24:53Z krasznaa $
#ifndef XAODCORE_SHALLOWAUXINFO_H
#define XAODCORE_SHALLOWAUXINFO_H

// Local include(s):
#include "xAODCore/ShallowAuxContainer.h"

namespace xAOD {

   /// Shallow copy for the auxiliary store of standalone objects
   ///
   /// The design of this is exactly the same as for the
   /// <code>SG::AuxStoreInternal</code> - <code>SG::AuxStoreStandalone</code>
   /// pair. All the code is in <code>xAOD::ShallowAuxContainer</code>, this
   /// class is just a convenience shorthand for calling
   /// <code>xAOD::ShallowAuxContainer(true)</code> in the code.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 611401 $
   /// $Date: 2014-08-12 14:24:53 +0200 (Tue, 12 Aug 2014) $
   ///
   class ShallowAuxInfo : public ShallowAuxContainer {

   public:
      /// Default constructor
      ShallowAuxInfo();
      /// Constructor with a parent object
      ShallowAuxInfo( const DataLink< SG::IConstAuxStore >& parent );

   }; // class ShallowAuxInfo

} // namespace xAOD

// Declare a class ID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ShallowAuxInfo, 196927374, 1 )

// Describe the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ShallowAuxInfo, xAOD::ShallowAuxContainer );

#endif // XAODCORE_SHALLOWAUXINFO_H
