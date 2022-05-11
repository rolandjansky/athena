/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMHITSTRUTHRELINK_H
#define MMHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <MuonSimEvent/MMSimHitCollection.h>


class MM_HitsTruthRelink : public HitsTruthRelinkBase
{
public:
  MM_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<MMSimHitCollection> m_inputHitsKey {this, "InputHits", "MM_HitsOLD", "Input MM hits name"};
  SG::WriteHandleKey<MMSimHitCollection> m_outputHitsKey {this, "OutputHits", "MM_Hits", "Output MM hits name"};
};

#endif
