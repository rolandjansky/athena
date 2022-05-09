/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCHITSTRUTHRELINK_H
#define STGCHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <MuonSimEvent/sTGCSimHitCollection.h>


class sTGC_HitsTruthRelink : public HitsTruthRelinkBase
{
public:
  sTGC_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<sTGCSimHitCollection> m_inputHitsKey {this, "InputHits", "sTGC_HitsOLD", "Input TGC hits name"};
  SG::WriteHandleKey<sTGCSimHitCollection> m_outputHitsKey {this, "OutputHits", "sTGC_Hits", "Output TGC hits name"};
};

#endif
