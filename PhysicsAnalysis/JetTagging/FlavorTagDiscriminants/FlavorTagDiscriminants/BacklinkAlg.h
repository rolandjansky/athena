/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BACKLINK_ALG_H
#define BACKLINK_ALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"

namespace FlavorTagDiscriminants {
  template <typename OLD, typename NEW>
  class BacklinkAlg : public AthReentrantAlgorithm
  {
  public:
    BacklinkAlg(const std::string& name, ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& cxt) const override;
    virtual StatusCode finalize() override;
  private:

    // Better would be to make everything work with AuxElement, e.g.
    //
    //  using EC = DataVector<SG::AuxElement>;
    //  using NEW = EC;
    //  using OLD = EC;
    //
    // But I haven't figured out how to make this work (I'd need to
    // assign a class ID, and even then I'm not sure it would be
    // supported).

    SG::ReadDecorHandleKey<OLD> m_oldLink {
      this, "oldLink", "", "Existing link"};
    SG::WriteDecorHandleKey<NEW> m_newLink {
      this, "newLink", "", "Link to be added"};
  };
}

#include "BacklinkAlg.icc"

#endif
