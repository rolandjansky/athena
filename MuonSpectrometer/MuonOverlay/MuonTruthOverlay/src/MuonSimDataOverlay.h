/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef MUONTRUTHOVERLAY_MUONSIMDATAOVERLAY_H
#define MUONTRUTHOVERLAY_MUONSIMDATAOVERLAY_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "MuonSimData/MuonSimDataCollection.h"

class MuonSimDataOverlay : public AthReentrantAlgorithm
{
public:
  MuonSimDataOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:

  SG::ReadHandleKey<MuonSimDataCollection> m_bkgInputKey{ this, "BkgInputKey", "", "ReadHandleKey for Background Input MuonSimDataCollection" };
  SG::ReadHandleKey<MuonSimDataCollection> m_signalInputKey{ this, "SignalInputKey", "", "ReadHandleKey for Signal Input MuonSimDataCollection" };
  SG::WriteHandleKey<MuonSimDataCollection> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output MuonSimDataCollection" };

};

#endif // MUONTRUTHOVERLAY_MUONSIMDATAOVERLAY_H
