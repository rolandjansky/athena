/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCHITSTRUTHRELINK_H
#define CSCHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <MuonSimEvent/CSCSimHitCollection.h>


class CSC_HitsTruthRelink : public HitsTruthRelinkBase
{
public:
  CSC_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<CSCSimHitCollection> m_inputHitsKey {this, "InputHits", "CSC_HitsOLD", "Input CSC hits name"};
  SG::WriteHandleKey<CSCSimHitCollection> m_outputHitsKey {this, "OutputHits", "CSC_Hits", "Output CSC hits name"};
};

#endif
