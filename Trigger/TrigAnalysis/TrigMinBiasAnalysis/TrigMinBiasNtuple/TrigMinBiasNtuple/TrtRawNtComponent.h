/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_TRTRAWNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_TRTRAWNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

class TRT_ID;

namespace NtComponent {
  class TrtRawNtComponent: public NtupleComponent {
  public:
    TrtRawNtComponent(NtupleAlgorithm *mainAlg,
		      NtComponentParameters parameters);
    virtual ~TrtRawNtComponent();

    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:
    /** For access to the TRT identifiers */
    const TRT_ID * m_trtHelper;

    /** The name of the TRT RDO container */
    std::string  m_trtRDO_ContKey;    

    // NTUPLE variables
    unsigned int m_nraw;
    std::vector<float> *m_raw_tot;
    std::vector<short> *m_raw_barrel_ec;
    std::vector<short> *m_raw_phi_module;
    std::vector<short> *m_raw_layer_or_wheel;
    std::vector<short> *m_raw_straw_layer;
    std::vector<short> *m_raw_straw;
  };
}

#endif
