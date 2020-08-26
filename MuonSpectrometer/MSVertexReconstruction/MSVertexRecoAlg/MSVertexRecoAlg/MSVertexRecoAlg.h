/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXRECOALG_H
#define MSVERTEXRECOALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MSVertexToolInterfaces/IMSVertexRecoTool.h"
#include "MSVertexToolInterfaces/IMSVertexTrackletTool.h"

class MSVertexRecoAlg : public AthReentrantAlgorithm {

 public:

  MSVertexRecoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MSVertexRecoAlg()=default;

  StatusCode initialize() override;
  StatusCode execute (const EventContext& ctx) const override;

 private:
  ToolHandle<Muon::IMSVertexTrackletTool> m_vertexTrackletTool{this,"MSVertexTrackletTool","Muon::MSVertexTrackletTool/MSVertexTrackletTool"};
  ToolHandle<Muon::IMSVertexRecoTool> m_vertexRecoTool{this,"MSVertexRecoTool","Muon::MSVertexRecoTool/MSVertexRecoTool"};

};

#endif
