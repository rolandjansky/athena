// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoIAuxContainer_v1.h 574653 2013-12-09 15:53:17Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_JETROIAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_JETROIAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for the LVL1 jet RoI container
   ///
   /// This auxiliary container can describe the properties of a container
   /// of LVL1 jet RoIs.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 574653 $
   /// $Date: 2013-12-09 16:53:17 +0100 (Mon, 09 Dec 2013) $
   ///
   class JetRoIAuxContainer_v1 : public AuxContainerBase{

   public:
      /// Default constuctor
      JetRoIAuxContainer_v1();

   private:
      std::vector< float > eta;
      std::vector< float > phi;
      std::vector< uint32_t > roiWord;

      std::vector< std::vector< float > > thrValues;
      std::vector< std::vector< std::string > > thrNames;

      std::vector< float > et4x4;
      std::vector< float > et6x6;
      std::vector< float > et8x8;

   }; // class JetRoIAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::JetRoIAuxContainer_v1, 1287744961, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::JetRoIAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_JETROIAUXCONTAINER_V1_H
