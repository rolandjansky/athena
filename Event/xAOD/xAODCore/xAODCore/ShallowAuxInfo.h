// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowAuxInfo.h 671678 2015-06-02 12:28:46Z krasznaa $
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
   /// $Revision: 671678 $
   /// $Date: 2015-06-02 14:28:46 +0200 (Tue, 02 Jun 2015) $
   ///
   class ShallowAuxInfo : public ShallowAuxContainer {

   public:
      /// Default constructor
      ShallowAuxInfo();
      /// Constructor with a parent object
      ShallowAuxInfo( const DataLink< SG::IConstAuxStore >& parent );

      /// Return the type of the store object
      virtual AuxStoreType getStoreType() const { return AST_ObjectStore; }

   }; // class ShallowAuxInfo

} // namespace xAOD

// Declare a class ID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ShallowAuxInfo, 196927374, 1 )

// Describe the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ShallowAuxInfo, xAOD::ShallowAuxContainer );

#endif // XAODCORE_SHALLOWAUXINFO_H
