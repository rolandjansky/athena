/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETLOWPTNTUPLE_TRACKNTCOMPONENT_H
#define INDETLOWPTNTUPLE_TRACKNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>
#include <map>

namespace NtComponent {
  class TrackNtComponent: public NtupleComponent {
  public:
    TrackNtComponent(NtupleAlgorithm *mainAlg,
		     NtComponentParameters parameters);
    virtual ~TrackNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_trackCollectionKey;

    // ntuple variables

    /** Basic track parameters */
    std::vector<float> *m_trk_d0;
    std::vector<float> *m_trk_z0;
    std::vector<float> *m_trk_phi0;
    std::vector<float> *m_trk_theta;
    std::vector<float> *m_trk_qOverP;
    std::vector<float> *m_trk_eta;
    std::vector<float> *m_trk_d0_err;
    std::vector<float> *m_trk_z0_err;
    std::vector<float> *m_trk_phi0_err;
    std::vector<float> *m_trk_theta0_err;
    std::vector<float> *m_trk_qOverP_err;
  };
}

#endif
