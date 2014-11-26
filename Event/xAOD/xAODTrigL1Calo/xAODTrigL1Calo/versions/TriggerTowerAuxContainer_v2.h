// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerAuxContainer_v2.h 631385 2014-11-26 21:27:40Z gwatts $
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
  /// $Revision: 631385 $
  /// $Date: 2014-11-26 22:27:40 +0100 (Wed, 26 Nov 2014) $
  ///
  class TriggerTowerAuxContainer_v2 : public AuxContainerBase {
  public:
    // Default constructor
    TriggerTowerAuxContainer_v2();
    
  private:
    
    // Tower identifiers
    std::vector<uint_least32_t> coolId;
    std::vector<uint_least8_t> layer;
    std::vector<float> eta;
    std::vector<float> phi;

    // Quantities with same number of slices as LUT data
    std::vector<std::vector<uint_least8_t> > lut_cp;
    std::vector<std::vector<uint_least8_t> > lut_jep;
    std::vector<std::vector<int_least16_t> > correction;
    std::vector<std::vector<uint_least8_t> > correctionEnabled;
    std::vector<std::vector<uint_least8_t> > bcidVec;

    // Quantities with same number of slices as ADC data
    std::vector<std::vector<uint_least16_t> > adc;
    std::vector<std::vector<uint_least8_t> > bcidExt;

    // One error word/tower/event
    std::vector<uint_least16_t> error;

    // Not part of readout, but to extract "peak" slice from vectors
    std::vector<uint_least8_t> peak;
    std::vector<uint_least8_t> adcPeak;     

    
  }; // class TriggerTowerAuxContainer_v2 
} // namespace xAOD

#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TriggerTowerAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERAUXCONTAINER_V2_H
