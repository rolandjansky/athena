/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_JET_DECORATOR_ALG_H
#define BTAG_JET_DECORATOR_ALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "FlavorTagDiscriminants/ISingleJetDecorator.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"

namespace FlavorTagDiscriminants {
  class BTagJetDecoratorAlg : public AthReentrantAlgorithm
  {
  public:
    BTagJetDecoratorAlg(const std::string& name, ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& cxt) const override;
    virtual StatusCode finalize() override;
  private:
    SG::ReadHandleKey<xAOD::BTaggingContainer> m_btagContainerKey {
      this, "btagContainer", "", "Key for the input btag collection"};
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackContainerKey {
      this, "trackContainer", "InDetTrackParticles",
        "Key for track particle container"};

    // get the jet from the b-tagging container, always called "jetLink"
    SG::ReadDecorHandleKey<xAOD::BTaggingContainer> m_jetAcc {
      this, "jetLinkName", "", "Link from btag container to jet"};

    ToolHandle<ISingleJetDecorator> m_decorator{
      this, "decorator", "", "Decorator tool"};

    // Keys to keep track of the inputs / outputs
    std::vector<SG::ReadDecorHandleKey<xAOD::BTaggingContainer>> m_btagAux;
    std::vector<SG::ReadDecorHandleKey<xAOD::TrackParticleContainer>> m_trkAux;
    std::vector<SG::WriteDecorHandleKey<xAOD::BTaggingContainer>> m_btagDecor;


  };
}

#endif
