/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SILICONHITSTRUTHRELINK_H
#define SILICONHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <InDetSimEvent/SiHitCollection.h>


class SiliconHitsTruthRelink : public HitsTruthRelinkBase
{
public:
  SiliconHitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<SiHitCollection> m_inputHitsKey {this, "InputHits", "", "Input silicon hits name"};
  SG::WriteHandleKey<SiHitCollection> m_outputHitsKey {this, "OutputHits", "", "Output silicon hits name"};
};

#endif
