/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBRECO_h
#define AFP_GLOBRECO_h



#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "AFP_GlobReco/IAFP_GlobRecoTool.h"


class AFP_GlobReco : public AthReentrantAlgorithm
{
  public:
    AFP_GlobReco(const std::string& name, ISvcLocator* pSvcLocator);
    ~AFP_GlobReco();

    StatusCode initialize();
    StatusCode execute(const EventContext &ctx) const;
    StatusCode finalize();
    
  private:
    /// Tool that does the track reconstruction
    ToolHandle<IAFP_GlobRecoTool> m_recoToolHandle {this, "recoTool", "", "AFP proton reco tool"};
};

#endif
