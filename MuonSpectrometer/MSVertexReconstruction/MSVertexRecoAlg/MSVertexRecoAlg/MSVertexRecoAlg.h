/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXRECOALG_H
#define MSVERTEXRECOALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MSVertexToolInterfaces/IMSVertexRecoTool.h"
#include "MSVertexToolInterfaces/IMSVertexTrackletTool.h"

class MSVertexRecoAlg : public AthReentrantAlgorithm 
{

 public:

  MSVertexRecoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MSVertexRecoAlg();

  StatusCode initialize() override;
  StatusCode execute_r (const EventContext& ctx) const override;
  StatusCode finalize() override;


 private:

  ToolHandle<Muon::IMSVertexTrackletTool> m_vertexTrackletTool;
  ToolHandle<Muon::IMSVertexRecoTool>     m_vertexRecoTool;

};

#endif
