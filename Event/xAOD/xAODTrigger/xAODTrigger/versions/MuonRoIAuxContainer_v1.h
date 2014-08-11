// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoIAuxContainer_v1.h 574653 2013-12-09 15:53:17Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_MUONROIAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_MUONROIAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

   /// Auxiliary store for the LVL1 muon RoI container
   ///
   /// This auxiliary container can describe the properties of a container
   /// of LVL1 muon RoIs.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 574653 $
   /// $Date: 2013-12-09 16:53:17 +0100 (Mon, 09 Dec 2013) $
   ///
   class MuonRoIAuxContainer_v1 : public AuxContainerBase{

   public:
      /// Default constuctor
      MuonRoIAuxContainer_v1();

   private:
      std::vector< float > eta;
      std::vector< float > phi;
      std::vector< uint32_t > roiWord;
      std::vector< float > thrValue;
      std::vector< std::string > thrName;

   }; // class MuonRoIAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::MuonRoIAuxContainer_v1, 1294319038, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::MuonRoIAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_MUONROIAUXCONTAINER_V1_H
