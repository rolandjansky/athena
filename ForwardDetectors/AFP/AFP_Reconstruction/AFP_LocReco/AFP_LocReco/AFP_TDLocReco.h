/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLOCRECO_H
#define AFP_TDLOCRECO_H

#include "AFP_LocRecoInterfaces/IAFPTDLocRecoTool.h"

#include "AFP_LocReco/AFP_TDLocRecoTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" 

#include <string>

/// Algorithm reconstructing tracks from hits clusters
class AFP_TDLocReco : public AthAlgorithm
{
public:
  AFP_TDLocReco(const std::string& name, ISvcLocator* pSvcLocator);

  /// Does nothing
  ~AFP_TDLocReco() override {}

  StatusCode initialize() override;

  /// Run pixel clustering tool and next run track reconstruction tools
  StatusCode execute() override;

  /// Does nothing
  StatusCode finalize() override;

private:
  /// Tool that does the track reconstruction
  ToolHandle<IAFPTDLocRecoTool> m_recoToolHandle; 
};

#endif	//AFP_TDLOCRECO_h

