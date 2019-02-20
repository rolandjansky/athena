/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_TRACK_AUGMENTER_ALG_HH
#define BTAGGING_TRACK_AUGMENTER_ALG_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"

namespace Trk  {
  class ITrackToVertexIPEstimator;
  class IExtrapolator;
}

namespace Analysis {
  class Vector3DDecorator {
  public:
    Vector3DDecorator(const std::string& prefix);
    void set(const SG::AuxElement& el, const Amg::Vector3D& vec) const;
  private:
    SG::AuxElement::Decorator< std::vector<float> > m_dec;
  };


  class BTagTrackAugmenterAlg: public AthAlgorithm {
  public:
    BTagTrackAugmenterAlg(const std::string& name,
                          ISvcLocator* pSvcLocator );

    StatusCode initialize() override final;
    StatusCode execute() override final;
    StatusCode finalize() override final;
  private:
    const xAOD::Vertex* getPrimaryVertex() const;
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_track_to_vx;
    ToolHandle< Trk::IExtrapolator >  m_extrapolator;

    // decorators for tracks
    SG::AuxElement::Decorator< float > m_dec_d0;
    SG::AuxElement::Decorator< float > m_dec_z0;
    SG::AuxElement::Decorator< float > m_dec_d0_sigma;
    SG::AuxElement::Decorator< float > m_dec_z0_sigma;

    // these are used to calculate the signing with respect to the jet
    Vector3DDecorator m_dec_track_pos;
    Vector3DDecorator m_dec_track_mom;

    // tagging variable prefix
    std::string m_prefix;
    std::string m_primaryVertexContainer;
  };

}

#endif
