/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCHITSTRUTHRELINK_H
#define RPCHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <MuonSimEvent/RPCSimHitCollection.h>


class RPC_HitsTruthRelink : public HitsTruthRelinkBase
{
public:
  RPC_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<RPCSimHitCollection> m_inputHitsKey {this, "InputHits", "RPC_HitsOLD", "Input RPC hits name"};
  SG::WriteHandleKey<RPCSimHitCollection> m_outputHitsKey {this, "OutputHits", "RPC_Hits", "Output RPC hits name"};
};

#endif
