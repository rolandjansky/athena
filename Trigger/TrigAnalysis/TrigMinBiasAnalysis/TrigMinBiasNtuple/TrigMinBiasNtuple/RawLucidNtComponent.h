/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_RAWLUCIDNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_RAWLUCIDNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>

namespace NtComponent {
  class RawLucidNtComponent: public NtupleComponent {
  public:
    RawLucidNtComponent(NtupleAlgorithm *mainAlg,
			NtComponentParameters parameters);
    virtual ~RawLucidNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_lucid_RawDataContainer_key;

    // NTUPLE variables
    int m_word0;
    int m_word1;
    int m_word2;
    int m_word3;
    int m_word0p;
    int m_word1p;
    int m_word2p;
    int m_word3p;
    int m_word0n;
    int m_word1n;
    int m_word2n;
    int m_word3n;
    int m_status;
  };
}

#endif
