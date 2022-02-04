// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_EFEXTAUROIAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_EFEXTAUROIAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for the LVL1 eFEX tau TOB/RoI container
   ///
   /// This auxiliary container can describe the properties of a container
   /// of LVL1 tau RoIs.
   ///
   /// @author Nicholas Luongo <nicholas.andrew.luongo@cern.ch>
   ///
   class eFexTauRoIAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constuctor
      eFexTauRoIAuxContainer_v1();

   private:
      std::vector< uint32_t > word0;
      std::vector< uint32_t > word1;
      std::vector< uint16_t > rCoreNumerator;
      std::vector< uint16_t > rCoreDenominator;
      std::vector< uint16_t > rHadNumerator;
      std::vector< uint16_t > rHadDenominator;

      std::vector< float >    et;
      std::vector< float >    eta;
      std::vector< float >    phi;
      std::vector< char >     isTOB;

   }; // class eFexTauRoIAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::eFexTauRoIAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_EFEXTAUROIAUXCONTAINER_V1_H
