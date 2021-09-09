/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCHITSTRUTHRELINK_H
#define TGCHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <MuonSimEvent/TGCSimHitCollection.h>


class TGC_HitsTruthRelink : public HitsTruthRelinkBase
{
public:
  TGC_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<TGCSimHitCollection> m_inputHitsKey {this, "InputHits", "TGC_HitsOLD", "Input TGC hits name"};
  SG::WriteHandleKey<TGCSimHitCollection> m_outputHitsKey {this, "OutputHits", "TGC_Hits", "Output TGC hits name"};
};

#endif
