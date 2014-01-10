/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_MBTSCONTAINERNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_MBTSCONTAINERNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

class TileTBID;

namespace NtComponent {
  class MbtsContainerNtComponent: public NtupleComponent {
  public:
    MbtsContainerNtComponent(NtupleAlgorithm *mainAlg,
			     NtComponentParameters parameters);
    virtual ~MbtsContainerNtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_mbtsContainerName;

    /** For access to the tile test beam identifiers */
    const TileTBID* m_tileTBID;

    // NTUPLE variables
    int m_nchan;
    std::vector<float>* m_energy;
    std::vector<float>* m_time;
    std::vector<float>* m_quality;
    std::vector<int>* m_type;
    std::vector<int>* m_channel;
    std::vector<int>* m_module;
  };
}

#endif
