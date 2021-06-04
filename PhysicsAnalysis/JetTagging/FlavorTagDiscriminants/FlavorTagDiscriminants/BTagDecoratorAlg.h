/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_DECORATOR_ALG_H
#define BTAG_DECORATOR_ALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "FlavorTagDiscriminants/IBTagDecorator.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"

namespace FlavorTagDiscriminants {
  class BTagDecoratorAlg : public AthReentrantAlgorithm
  {
  public:
    BTagDecoratorAlg(const std::string& name, ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& cxt) const override;
    virtual StatusCode finalize() override;
  private:
    SG::ReadHandleKey<xAOD::BTaggingContainer> m_btagContainerKey {
      this, "btagContainer", "", "Key for the input btag collection"};
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackContainerKey {
      this, "trackContainer", "InDetTrackParticles",
        "Key for track particle container"};
    Gaudi::Property<std::vector<std::string>> m_undeclaredReadDecorKeys {
      this, "undeclaredReadDecorKeys", {},
      "List of read handles that we don't read, e.g. static variables" };

    ToolHandle<IBTagDecorator> m_decorator{
      this, "decorator", "", "Decorator tool"};

    // Keys to keep track of the inputs / outputs
    std::vector<SG::ReadDecorHandleKey<xAOD::BTaggingContainer>> m_btagAux;
    std::vector<SG::ReadDecorHandleKey<xAOD::TrackParticleContainer>> m_trkAux;
    std::vector<SG::WriteDecorHandleKey<xAOD::BTaggingContainer>> m_btagDecor;


  };
}

#endif
