// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_VERSIONS_TRIGRINGERAUXCONTAINER_V2_H
#define XAODTRIGRINGER_VERSIONS_TRIGRINGERAUXCONTAINER_V2_H

// System include(s):
extern "C" {
  #include <stdint.h>
}
// STL include(s):
#include <vector>
#include "AthLinks/ElementLink.h"
// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

namespace xAOD {
/// This is a fixed implementation of the trigger TrigRingerRings
/// auxiliary store. Defining every possible property that TrigRingerRings
///
/// @author Joao victor da Fonseca Pinto <joao.victor.da.fonseca.pinto@cern.ch>
///
/// $Revsision$
/// $Date: 2014-17-10$
///
   class TrigRingerRingsAuxContainer_v2: public AuxContainerBase{

   public:
      /// Default constructor
      TrigRingerRingsAuxContainer_v2();

   private:
      std::vector< std::vector< float > > rings; // Holds the generated rings.
      /// name Links to the electron constituents
      std::vector< ElementLink< TrigEMClusterContainer > > emClusterLink;
      
   }; 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigRingerRingsAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGSAUXCONTAINER_V1_H
