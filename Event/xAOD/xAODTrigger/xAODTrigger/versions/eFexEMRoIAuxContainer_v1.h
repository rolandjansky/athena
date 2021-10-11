// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: eFexEMRoIAuxContainer_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_EFEXEMROIAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_EFEXEMROIAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for the LVL1 eFEX EM TOB/RoI container
   ///
   /// This auxiliary container can describe the properties of a container
   /// of LVL1 EM RoIs.
   ///
   /// @author Alan Watson <Alan.Watson@cern.ch>
   ///
   /// $Revision: 1 $
   /// $Date: 2019-07-01 13:04:25 +0100 (Mon, 01 Jul 2019) $
   ///
   class eFexEMRoIAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constuctor
      eFexEMRoIAuxContainer_v1();

   private:
      std::vector< uint32_t > word0;
      std::vector< uint32_t > word1;
      std::vector< uint16_t > RetaEnv;
      std::vector< uint16_t > RetaCore;
      std::vector< uint16_t > RhadHad;
      std::vector< uint16_t > RhadEM;
      std::vector< uint16_t > WstotNumerator;
      std::vector< uint16_t > WstotDenominator;

      std::vector< float >    et;
      std::vector< float >    eta;
      std::vector< float >    phi;
      std::vector< char >     isTOB;

   }; // class eFexEMRoIAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::eFexEMRoIAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_eFexEMRoIAuxContainer_V1_H
