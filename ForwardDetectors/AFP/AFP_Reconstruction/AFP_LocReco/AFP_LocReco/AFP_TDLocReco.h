/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLOCRECO_H
#define AFP_TDLOCRECO_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/errorcheck.h" // CHECK( )
#include "AthenaBaseComps/AthMsgStreamMacros.h" 
#include "AFP_LocReco/IAFPTDLocRecoTool.h"

#include <string>

/// Algorithm reconstructing tracks from hits clusters
class AFP_TDLocReco : public AthReentrantAlgorithm
{
public:
  AFP_TDLocReco(const std::string& name, ISvcLocator* pSvcLocator);

  /// Does nothing
  ~AFP_TDLocReco() {}

  StatusCode initialize();

  /// Run pixel clustering tool and next run track reconstruction tools
  StatusCode execute(const EventContext &ctx) const;

  /// Does nothing
  StatusCode finalize();

private:
  /// Tool that does the track reconstruction
  ToolHandle<IAFPTDLocRecoTool> m_recoToolHandle{this, "recoTool", "", "Tool that makes all ToF tracks reconstruction"};
};

#endif	//AFP_TDLOCRECO_h
