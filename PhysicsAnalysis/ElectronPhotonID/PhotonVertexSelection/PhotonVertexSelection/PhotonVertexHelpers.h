/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_PhotonVertexHelpers_H
#define PhotonVertexSelection_PhotonVertexHelpers_H

// EDM includes
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEventInfo/EventInfo.h"

// ROOT includes
#include "TLorentzVector.h"

class ShowerDepthTool;

namespace xAOD {
  namespace PVHelpers {

    ///@brief Return zCommon and zCommonError
    std::pair<float, float> getZCommonAndError(const xAOD::EventInfo *eventInfo,
                                               const xAOD::EgammaContainer *egammas,
                                               float convPtCut = 2e3);

    ///@brief Loop over track particles associated with vertex and return scalar sum of pT^power in GeV (from auxdata if available and useAux = true)
    float getVertexSumPt(const xAOD::Vertex *vertex, int power = 1, bool useAux = true);

    ///@brief Return vector sum of tracks associated with vertex (from auxdata if available and useAux = true)
    TLorentzVector getVertexMomentum(const xAOD::Vertex *vertex, bool useAux = true, std::string derivationPrefix = "");

    ///@brief Return vertex with highest sum pT^2
    const xAOD::Vertex* getHardestVertex(const xAOD::VertexContainer *vertices);

    ///@brief Return track at first measurment, useful as proxy to conversion vertex pT
    TLorentzVector getTrackAtFirstMeasurement(const xAOD::TrackParticle *tp);

    ///@brief Check if photon is converted, and tracks have Si hits and pass selection
    bool passConvSelection(const xAOD::Photon *photon,
                           float convPtCut = 2e3);

    ///@brief Check if track i of conversion vertex pass selection (Si hits and pt)
    bool passConvSelection(const xAOD::Vertex& conversionVertex,
                           size_t i,
                           float convPtCut);

  }
}


#endif // PhotonVertexSelection_PhotonVertexHelpers_H
