/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_TRUTHLUCIDLVL1NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_TRUTHLUCIDLVL1NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>

namespace NtComponent {
  class TruthLucidLvl1NtComponent: public NtupleComponent {
  public:
    TruthLucidLvl1NtComponent(NtupleAlgorithm *mainAlg,
			      NtComponentParameters parameters);
    virtual ~TruthLucidLvl1NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_lucid_DigitContainer_key;

    // NTUPLE variables
    int m_ntube;
    std::vector<short>* m_side;
    std::vector<short>* m_tubeId;
    std::vector<double>* m_npe;
    std::vector<int>* m_npeGas;
    std::vector<int>* m_npePmt;
    std::vector<int>* m_qdc;
    std::vector<int>* m_tdc;
    std::vector<bool>* m_isHit;
  };
}

#endif
