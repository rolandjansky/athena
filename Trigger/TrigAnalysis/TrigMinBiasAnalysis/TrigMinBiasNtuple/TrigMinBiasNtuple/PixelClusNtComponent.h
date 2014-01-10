/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_PIXELCLUSNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_PIXELCLUSNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

class PixelID;

namespace NtComponent {
  class PixelClusNtComponent: public NtupleComponent {
  public:
    PixelClusNtComponent(NtupleAlgorithm *mainAlg,
			 NtComponentParameters parameters);
    virtual ~PixelClusNtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:
    /** The name of the Pixel cluster container */
    std::string  m_pixelSiClustersKey;
   
    /** For access to the Pixel identifiers */
    const PixelID * m_pixelHelper;

    // NTUPLE variables
    int m_nclus;
    std::vector<short> *m_clus_barrel_ec;
    std::vector<short> *m_clus_layer_disk;
    std::vector<short> *m_clus_phi_module;
    std::vector<short> *m_clus_eta_module;
    std::vector<int>   *m_clus_hashid_module; // unique module id
    std::vector<int>   *m_clus_size;
    std::vector<int>   *m_clus_tot;
    std::vector<float> *m_clus_charge;
    std::vector<bool>  *m_clus_ganged;
    std::vector<float> *m_clus_global_x;
    std::vector<float> *m_clus_global_y;
    std::vector<float> *m_clus_global_z;
  };
}

#endif
