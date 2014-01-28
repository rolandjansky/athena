// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerAuxContainer_v1.h 576243 2013-12-19 11:24:27Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// AuxContainer for TriggerTower_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 576243 $
  /// $Date: 2013-12-19 12:24:27 +0100 (Thu, 19 Dec 2013) $
  ///
  class TriggerTowerAuxContainer_v1 : public AuxContainerBase {
  public:
    // Default constructor
    TriggerTowerAuxContainer_v1();
    
  private:
    std::vector<std::vector<int> > emLUT;
    std::vector<std::vector<int> > hadLUT;
    std::vector<std::vector<int> > emADC;
    std::vector<std::vector<int> > hadADC;   
    std::vector<std::vector<int> > emBCIDvec;
    std::vector<std::vector<int> > hadBCIDvec;
    std::vector<std::vector<int> > emBCIDext;
    std::vector<std::vector<int> > hadBCIDext;
    std::vector<float> phi;
    std::vector<float> eta;
    std::vector<unsigned int> key;
    std::vector<int> emError;
    std::vector<int> hadError;
    std::vector<int> emPeak;
    std::vector<int> emADCPeak;
    std::vector<int> hadPeak;
    std::vector<int> hadADCPeak;
    
  }; // class TriggerTowerAuxContainer_v1 
} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::TriggerTowerAuxContainer_v1 , 1086692909 , 1 ) 
      
#include "SGTools/BaseInfo.h"
  SG_BASE( xAOD::TriggerTowerAuxContainer_v1 , xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERAUXCONTAINER_V1_H
