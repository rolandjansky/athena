/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "SimpleMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"

using namespace Muon;

Muon::SimpleMMClusterBuilderTool::SimpleMMClusterBuilderTool(const std::string& t,
							     const std::string& n,
							     const IInterface*  p )
  :  
  AthAlgTool(t,n,p)
{


}

Muon::SimpleMMClusterBuilderTool::~SimpleMMClusterBuilderTool()
{

}


StatusCode Muon::SimpleMMClusterBuilderTool::initialize()
{

  return StatusCode::SUCCESS;
}


StatusCode Muon::SimpleMMClusterBuilderTool::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode Muon::SimpleMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& stripsVect, 
							 std::vector<Muon::MMPrepData>& clustersVect)

{

  ATH_MSG_DEBUG("Size of the input vector: " << stripsVect.size()); 
  ATH_MSG_DEBUG("Size of the output vector: " << clustersVect.size()); 


  return StatusCode::SUCCESS;
}
