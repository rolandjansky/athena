/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_PIXELSPNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_PIXELSPNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

class PixelID;

namespace NtComponent {
  class PixelSpNtComponent: public NtupleComponent {
  public:
    PixelSpNtComponent(NtupleAlgorithm *mainAlg,
		     NtComponentParameters parameters);
    virtual ~PixelSpNtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:

    /** SCT Space Points container key */
    std::string  m_pixelSpacePointsKey;
    
    const PixelID *m_pixelHelper;

    // NTUPLE variables
    int m_nsp;
    std::vector<short> *m_sp_barrel_ec;
    std::vector<short> *m_sp_layer_disk;
    std::vector<short> *m_sp_phi_module;
    std::vector<short> *m_sp_eta_module;
    std::vector<int>   *m_sp_hashid_module; // unique module id
    std::vector<float> *m_sp_global_x;
    std::vector<float> *m_sp_global_y;
    std::vector<float> *m_sp_global_z;
  };
}

#endif
