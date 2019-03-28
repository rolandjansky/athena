/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef MUONTRUTHOVERLAY_CSCSIMDATAOVERLAY_H
#define MUONTRUTHOVERLAY_CSCSIMDATAOVERLAY_H

#include <AthenaBaseComps/AthAlgorithm.h>
#include <MuonSimData/CscSimDataCollection.h>

class CscSimDataOverlay : public AthAlgorithm
{
public:
  CscSimDataOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  SG::ReadHandleKey<CscSimDataCollection> m_bkgInputKey;
  SG::ReadHandleKey<CscSimDataCollection> m_signalInputKey;
  SG::WriteHandleKey<CscSimDataCollection> m_outputKey;

};

#endif // MUONTRUTHOVERLAY_CSCSIMDATAOVERLAY_H
