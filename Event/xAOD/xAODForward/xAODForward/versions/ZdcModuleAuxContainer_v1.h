// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataAuxContainer_v1.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_VERSIONS_ZDCMODULEAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_ZDCMODULEAUXCONTAINER_V1_H

// System include(s):
#include <vector>
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace xAOD {

   /// Auxiliary store for xAOD::ZdcModuleContainer_v1
   ///
   /// This is a custom auxiliary container for xAOD::ZdcModuleContainer_v1,
   /// to be used in the main reconstruction code.
   ///
   /// @author Peter Steinberg <peter.steinberg@bnl.gov>
   ///
   /// $Revision: 693858 $
   /// $Date: 2015-09-09 12:30:15 +0200 (Wed, 09 Sep 2015) $
   ///
   class ZdcModuleAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      ZdcModuleAuxContainer_v1();

   private:
     /// @name Information from LocRecEvCollection and LocRecODEvCollection
     /// @{
     //std::vector<uint64_t> id;
     //std::vector<uint32_t> id;
     std::vector<int> id;
     std::vector<int> side;
     std::vector<int> zdcModule;
     std::vector<int> type;
     std::vector<int> channel;

     std::vector< std::vector<float> > waveformTime;
     std::vector< std::vector<float> > waveformADC;
     std::vector<float> amplitude;
     std::vector<float> energy;
     std::vector<float> time;
     std::vector<float> qual;

     std::vector< std::vector<float> > waveformTimeG0;
     std::vector< std::vector<float> > waveformADCG0;
     std::vector<float> amplitudeG0;
     std::vector<float> energyG0;
     std::vector<float> timeG0;
     std::vector<float> qualG0;

     std::vector< std::vector<float> > waveformTimeG1;
     std::vector< std::vector<float> > waveformADCG1;
     std::vector<float> amplitudeG1;
     std::vector<float> energyG1;
     std::vector<float> timeG1;
     std::vector<float> qualG1;

     std::vector< ElementLink< TriggerTowerContainer > > TTg0d0Link;
     std::vector< ElementLink< TriggerTowerContainer > > TTg0d1Link;
     std::vector< ElementLink< TriggerTowerContainer > > TTg1d0Link;
     std::vector< ElementLink< TriggerTowerContainer > > TTg1d1Link;
     /// @}
     

   }; // class ZdcModuleAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ZdcModuleAuxContainer_v1, xAOD::AuxContainerBase );

#endif //XAODFORWARD_VERSIONS_ZDCMODULEAUXCONTAINER_V1_H
