/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLOCRECO_H
#define AFP_SIDLOCRECO_H

#include "AFP_LocRecoInterfaces/IAFPSiDLocRecoTool.h"

#include "AFP_LocReco/AFP_SIDLocRecoTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" 

#include <string>

/// Algorithm reconstructing tracks from hits clusters
class AFP_SIDLocReco : public AthAlgorithm
{
public:
  AFP_SIDLocReco(const std::string& name, ISvcLocator* pSvcLocator);

  /// Does nothing
  ~AFP_SIDLocReco() override {}

  StatusCode initialize() override;

  /// Run pixel clustering tool and next run track reconstruction tools
  StatusCode execute() override;

  /// Does nothing
  StatusCode finalize() override;

private:
  /// Tool that does the track reconstruction
  ToolHandle<IAFPSiDLocRecoTool> m_recoToolHandle; 
};

#endif	//AFP_TDLOCRECO_h
