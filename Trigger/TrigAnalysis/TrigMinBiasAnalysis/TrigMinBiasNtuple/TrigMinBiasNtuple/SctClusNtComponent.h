/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_SCTCLUSNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_SCTCLUSNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

class SCT_ID;

namespace NtComponent {
  class SctClusNtComponent: public NtupleComponent {
  public:
    SctClusNtComponent(NtupleAlgorithm *mainAlg,
		       NtComponentParameters parameters);
    virtual ~SctClusNtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:

    /** SCT Cluster container key */
    std::string  m_sctSiClustersKey;
    
    const SCT_ID *m_sctHelper;

    // NTUPLE variables
    int m_nclus;
    std::vector<short> *m_clus_barrel_ec;
    std::vector<short> *m_clus_layer_disk;
    std::vector<short> *m_clus_phi_module;
    std::vector<short> *m_clus_eta_module;
    std::vector<int>   *m_clus_hashid_module;
    std::vector<int>   *m_clus_size;
    std::vector<float> *m_clus_global_x;
    std::vector<float> *m_clus_global_y;
    std::vector<float> *m_clus_global_z;
  };
}

#endif
