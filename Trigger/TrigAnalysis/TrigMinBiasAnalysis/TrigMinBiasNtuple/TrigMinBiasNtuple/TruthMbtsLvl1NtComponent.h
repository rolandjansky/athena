/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_TRUTHMBTSLVL1NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_TRUTHMBTSLVL1NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

class TileTBID;

namespace NtComponent {
  class TruthMbtsLvl1NtComponent: public NtupleComponent {
  public:
    TruthMbtsLvl1NtComponent(NtupleAlgorithm *mainAlg,
			     NtComponentParameters parameters);
    virtual ~TruthMbtsLvl1NtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_tileTTL1ContainerKey;

    /** For access to the tile test beam identifiers */
    const TileTBID* m_tileTBID;

    // NTUPLE variables
    int m_ttl1_nchan;
    std::vector<int>* m_ttl1_side;
    std::vector<int>* m_ttl1_eta;
    std::vector<int>* m_ttl1_phi;
    std::vector<std::vector<double> >* m_ttl1_samples;
  };
}

#endif
