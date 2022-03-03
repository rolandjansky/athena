/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_VERTEXRECO_h
#define AFP_VERTEXRECO_h



#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "AFP_VertexReco/IAFP_VertexRecoTool.h"


class AFP_VertexReco : public AthReentrantAlgorithm
{
  public:
    AFP_VertexReco(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~AFP_VertexReco() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;
    
  private:
    /// Tool that does the vertex reconstruction
    ToolHandle<IAFP_VertexRecoTool> m_recoToolHandle {this, "recoTool", "", "AFP vertex reco tool"};
};

#endif
