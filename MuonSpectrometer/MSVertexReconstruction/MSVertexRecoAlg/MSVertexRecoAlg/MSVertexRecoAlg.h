/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXRECOALG_H
#define MSVERTEXRECOALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MSVertexToolInterfaces/IMSVertexRecoTool.h"
#include "MSVertexToolInterfaces/IMSVertexTrackletTool.h"

class MSVertexRecoAlg : public AthAlgorithm 
{

 public:

  MSVertexRecoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MSVertexRecoAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle<Muon::IMSVertexTrackletTool> m_vertexTrackletTool;
  ToolHandle<Muon::IMSVertexRecoTool>     m_vertexRecoTool;

};

#endif
