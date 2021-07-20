/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTHITSTRUTHRELINK_H
#define MDTHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <MuonSimEvent/MDTSimHitCollection.h>


class MDT_HitsTruthRelink : public HitsTruthRelinkBase
{
public:
  MDT_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<MDTSimHitCollection> m_inputHitsKey {this, "InputHits", "MDT_HitsOLD", "Input MDT hits name"};
  SG::WriteHandleKey<MDTSimHitCollection> m_outputHitsKey {this, "OutputHits", "MDT_Hits", "Output MDT hits name"};
};

#endif
