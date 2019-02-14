/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef MUONTRUTHOVERLAY_MUONSIMDATAOVERLAY_H
#define MUONTRUTHOVERLAY_MUONSIMDATAOVERLAY_H

#include <AthenaBaseComps/AthAlgorithm.h>
#include <MuonSimData/MuonSimDataCollection.h>

class MuonSimDataOverlay : public AthAlgorithm
{
public:
  MuonSimDataOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  SG::ReadHandleKey<MuonSimDataCollection> m_bkgInputKey{ this, "BkgInputKey", "", "ReadHandleKey for Background Input MuonSimDataCollection" };
  SG::ReadHandleKey<MuonSimDataCollection> m_signalInputKey{ this, "SignalInputKey", "", "ReadHandleKey for Signal Input MuonSimDataCollection" };
  SG::WriteHandleKey<MuonSimDataCollection> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output MuonSimDataCollection" };

};

#endif // MUONTRUTHOVERLAY_MUONSIMDATAOVERLAY_H
