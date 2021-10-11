/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_TRACK_LINK_COPY_ALG_H
#define BTAG_TRACK_LINK_COPY_ALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"

namespace FlavorTagDiscriminants {
  class BTagTrackLinkCopyAlg : public AthReentrantAlgorithm
  {
  public:
    BTagTrackLinkCopyAlg(const std::string& name, ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& cxt) const override;
    virtual StatusCode finalize() override;
  private:

    // Typedef things here in case I ever want to make this more generic.
    using BC = xAOD::BTaggingContainer;
    using JC = xAOD::JetContainer;

    SG::ReadDecorHandleKey<JC> m_jetTracks {
      this, "jetTracks", "", "Links to be copied"};
    SG::WriteDecorHandleKey<BC> m_btagTracks {
      this, "btagTracks", "", "Destination for track links"};
    SG::ReadDecorHandleKey<BC> m_jetLinks {
      this, "jetLinkName", "", "Destination container name + '.jetLink'"};
  };
}

#endif
