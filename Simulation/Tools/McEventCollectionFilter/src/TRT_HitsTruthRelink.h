/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTHITSTRUTHRELINK_H
#define TRTHITSTRUTHRELINK_H

// Base class include
#include "HitsTruthRelinkBase.h"

#include <InDetSimEvent/TRTUncompressedHitCollection.h>


class TRT_HitsTruthRelink : public HitsTruthRelinkBase
{
public:
  TRT_HitsTruthRelink(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  SG::ReadHandleKey<TRTUncompressedHitCollection> m_inputHitsKey {this, "InputHits", "TRTUncompressedHitsOLD", "Input TRT hits name"};
  SG::WriteHandleKey<TRTUncompressedHitCollection> m_outputHitsKey {this, "OutputHits", "TRTUncompressedHits", "Output TRT hits name"};

  Gaudi::Property<bool> m_keepElectronsLinkedToTRTHits {this, "KeepElectronsLinkedToTRTHits", false, "Keep electrons linked to TRT hits"};
};

#endif
