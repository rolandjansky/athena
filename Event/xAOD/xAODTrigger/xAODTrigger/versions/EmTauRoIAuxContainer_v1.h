// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoIAuxContainer_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_EMTAUROIAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_EMTAUROIAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for the LVL1 em/tau RoI container
   ///
   /// This auxiliary container can describe the properties of a container
   /// of LVL1 em/tau RoIs.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class EmTauRoIAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constuctor
      EmTauRoIAuxContainer_v1();

   private:
      std::vector< float > eta;
      std::vector< float > phi;
      std::vector< uint32_t > roiWord;

      std::vector< std::vector< float > > thrValues;
      std::vector< std::vector< std::string > > thrNames;

      std::vector< float > core;
      std::vector< float > emClus;
      std::vector< float > tauClus;
      std::vector< float > emIsol;
      std::vector< float > hadIsol;
      std::vector< float > hadCore;

   }; // class EmTauRoIAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EmTauRoIAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_EMTAUROIAUXCONTAINER_V1_H
