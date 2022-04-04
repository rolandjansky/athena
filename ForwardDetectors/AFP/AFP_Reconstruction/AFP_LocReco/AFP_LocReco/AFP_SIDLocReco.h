/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLOCRECO_h
#define AFP_SIDLOCRECO_h

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <fstream>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/errorcheck.h" // CHECK( )
#include "AthenaBaseComps/AthMsgStreamMacros.h" 
// #include "AFP_Geometry/AFP_GeometryTool.h"

#include "AFP_LocReco/IAFP_SIDLocRecoTool.h"


class AFP_SIDLocReco : public AthReentrantAlgorithm
{
  public:
    AFP_SIDLocReco(const std::string& name, ISvcLocator* pSvcLocator);
    ~AFP_SIDLocReco();

    StatusCode initialize();
    StatusCode execute(const EventContext &ctx) const;
    StatusCode finalize();
    
  private:
    /// Tool that does the track reconstruction
    ToolHandle<IAFP_SIDLocRecoTool> m_recoToolHandle {this, "recoTool", "", "AFP track reco tool"};
};

#endif
