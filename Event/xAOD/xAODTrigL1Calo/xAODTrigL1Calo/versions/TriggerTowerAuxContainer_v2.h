// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerAuxContainer_v2.h 652807 2015-03-09 21:52:07Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERAUXCONTAINER_V2_H

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// AuxContainer for TriggerTower_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 652807 $
  /// $Date: 2015-03-09 22:52:07 +0100 (Mon, 09 Mar 2015) $
  ///
  class TriggerTowerAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    TriggerTowerAuxContainer_v2();
    
  private:   
    
    // Tower identifiers
    std::vector<float> eta;           
    std::vector<float> phi; 
    std::vector<uint32_t> coolId;

    // Quantities with same number of slices as LUT data
    std::vector<std::vector<uint8_t> > lut_cp;
    std::vector<std::vector<uint8_t> > lut_jep;
    std::vector<std::vector<int16_t> > correction;
    std::vector<std::vector<uint8_t> > correctionEnabled;
    std::vector<std::vector<uint8_t> > bcidVec;

    // Quantities with same number of slices as ADC data
    std::vector<std::vector<uint16_t> > adc;
    std::vector<std::vector<uint8_t> > bcidExt;
    std::vector<std::vector<uint8_t> > sat80Vec;
    

    // One error word/tower/event
    std::vector<uint32_t> errorWord;

    // Not part of readout, but to extract "peak" slice from vectors
    std::vector<uint8_t> peak;
    std::vector<uint8_t> adcPeak;     

    
  }; // class TriggerTowerAuxContainer_v2 
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TriggerTowerAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERAUXCONTAINER_V2_H
