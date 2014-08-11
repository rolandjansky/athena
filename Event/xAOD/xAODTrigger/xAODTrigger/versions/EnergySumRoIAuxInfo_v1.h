// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoIAuxInfo_v1.h 574653 2013-12-09 15:53:17Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_ENERGYSUMROIAUXINFO_V1_H
#define XAODTRIGGER_VERSIONS_ENERGYSUMROIAUXINFO_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   /// Auxiliary store for an EnergySum RoI object
   ///
   /// This auxiliary store describes a single EnergySum RoI object, as we
   /// only have one of those in the ATLAS events.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 574653 $
   /// $Date: 2013-12-09 16:53:17 +0100 (Mon, 09 Dec 2013) $
   ///
   class EnergySumRoIAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constuctor
      EnergySumRoIAuxInfo_v1();

   private:
      uint32_t roiWord0;
      uint32_t roiWord1;
      uint32_t roiWord2;

      std::vector< std::string > thrNames;

      float energyX;
      float energyY;
      float energyT;

   }; // class EnergySumRoIAuxInfo_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::EnergySumRoIAuxInfo_v1, 263007227, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::EnergySumRoIAuxInfo_v1, xAOD::AuxInfoBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_ENERGYSUMROIAUXINFO_V1_H
