/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "SimpleSTgcClusterBuilderTool.h"
#include "MuonPrepRawData/sTgcPrepData.h"

#include <set>
#include <vector>

using namespace Muon;
using namespace std;

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
  StoreGateSvc* detStore=0;
  ATH_CHECK(serviceLocator()->service("DetectorStore",detStore));

  ATH_CHECK(detStore->retrieve(m_muonMgr));  
  m_stgcIdHelper = m_muonMgr->stgcIdHelper();

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

  // clear the clusters vector
  m_clusters.clear();
  m_clustersStripNum.clear();

  for ( auto& it : stripsVect ) {
    
    if ( !addStrip(it) ) {
      ATH_MSG_ERROR("Could not add a strip to the sTGC clusters");
      return StatusCode::FAILURE;
    }

  } 

  /// now add the clusters to the PRD container


  return StatusCode::SUCCESS;
}


bool Muon::SimpleSTgcClusterBuilderTool::addStrip(Muon::sTgcPrepData& strip)
{

  unsigned int stripNum = m_stgcIdHelper->channel(strip.identify());

  // if no cluster is present start creating a new one
  if ( m_clusters.size()==0 ) {

    set<unsigned int> clusterStripNum;
    vector<Muon::sTgcPrepData> cluster;

    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    m_clustersStripNum.push_back(clusterStripNum);
    m_clusters.push_back(cluster);

    return true;
  }
  else {
    for ( unsigned int i=0 ; i<m_clustersStripNum.size() ; ++i  ) {
      unsigned int firstStrip = *(m_clustersStripNum.at(i).begin());
      unsigned int lastStrip  = *(m_clustersStripNum.at(i).end());

      if ( stripNum==lastStrip+1 || stripNum==firstStrip-1 ) {

	m_clustersStripNum.at(i).insert(stripNum);
	m_clusters.at(i).push_back(strip);

	return true;
      }

    }
  }

  return false;
} 
