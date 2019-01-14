/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "SimpleSTgcClusterBuilderTool.h"
#include "MuonPrepRawData/sTgcPrepData.h"

using namespace Muon;

Muon::SimpleSTgcClusterBuilderTool::SimpleSTgcClusterBuilderTool(const std::string& t,
								 const std::string& n,
								 const IInterface*  p )
  :  
  AthAlgTool(t,n,p)
{


}

Muon::SimpleSTgcClusterBuilderTool::~SimpleSTgcClusterBuilderTool()
{

}


StatusCode Muon::SimpleSTgcClusterBuilderTool::initialize()
{

  return StatusCode::SUCCESS;
}


StatusCode Muon::SimpleSTgcClusterBuilderTool::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode Muon::SimpleSTgcClusterBuilderTool::getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, 
							   std::vector<Muon::sTgcPrepData*>& clustersVect)

{

  ATH_MSG_DEBUG("Size of the input vector: " << stripsVect.size()); 
  ATH_MSG_DEBUG("Size of the output vector: " << clustersVect.size()); 


  return StatusCode::SUCCESS;
}
