/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETLOWPTNTUPLE_TRACKLETNTCOMPONENT_H
#define INDETLOWPTNTUPLE_TRACKLETNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>
#include <map>

namespace NtComponent {
  class TrackletNtComponent: public NtupleComponent {
  public:
    TrackletNtComponent(NtupleAlgorithm *mainAlg,
		     NtComponentParameters parameters);
    virtual ~TrackletNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_trackParticleContainerKey;

    // ntuple variables

    /** Basic track parameters */
    int m_ntrkl;
    std::vector<float> *m_trkl_eta;
    std::vector<float> *m_trkl_phi;
    std::vector<float> *m_trkl_deta;
    std::vector<float> *m_trkl_dphi;
  };
}

#endif
