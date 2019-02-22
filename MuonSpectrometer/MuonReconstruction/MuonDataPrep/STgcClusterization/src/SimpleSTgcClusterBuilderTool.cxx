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
//
// Build the clusters given a vector of single-hit PRD
//
StatusCode Muon::SimpleSTgcClusterBuilderTool::getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, 
							   std::vector<Muon::sTgcPrepData*>& clustersVect)
{

  ATH_MSG_DEBUG("Size of the input vector: " << stripsVect.size()); 

  IdentifierHash hash;
  if ( stripsVect.size()>0 ) {
    hash = stripsVect.at(0).collectionHash();
  } 
  else {
    ATH_MSG_DEBUG("Empty PRD collection: no clusterization");
    return StatusCode::SUCCESS;
  }

  // clear the clusters vector
  for ( unsigned int multilayer =0 ; multilayer<3 ; ++multilayer ) {
    for ( unsigned int gasGap=0 ; gasGap<5 ; ++gasGap ) {
      m_clusters[multilayer][gasGap].clear();   
      m_clustersStripNum[multilayer][gasGap].clear();
    }
  }

  for ( auto& it : stripsVect ) {
    
    if ( !addStrip(it) ) {
      ATH_MSG_ERROR("Could not add a strip to the sTGC clusters");
      return StatusCode::FAILURE;
    }
  } 

  /// now add the clusters to the PRD container
  //
  clustersVect.clear();

  for ( unsigned int multilayer =0 ; multilayer<3 ; ++multilayer ) {
    for ( unsigned int gasGap=0 ; gasGap<5 ; ++gasGap ) {
      //
      // loop on the clusters of that gap
      //
      for ( unsigned int i=0 ; i<m_clusters[multilayer][gasGap].size() ; ++i ) { 
        // get the cluster
        std::vector<Muon::sTgcPrepData> cluster = m_clusters[multilayer][gasGap].at(i);
        //
        // loop on the strips and set the cluster weighted position and charge
        //
        std::vector<Identifier> rdoList;
        Identifier clusterId;
        double weightedPosX = 0.0;
        double posY = (cluster.at(0)).localPosition().y();

        double maxCharge = -1.0;
        double totalCharge  = 0.0;
        for ( auto it : cluster ) {
          rdoList.push_back(it.identify());
          weightedPosX += it.localPosition().x()*it.charge();
          totalCharge += it.charge();
          //
          // Set the cluster identifier to the max charge strip
          //
          if ( it.charge()>maxCharge ) {
            maxCharge = it.charge();
            clusterId = it.identify();
          } 
        } 
        weightedPosX = weightedPosX/totalCharge;
        Amg::Vector2D localPosition(weightedPosX,posY);        
        //
        // get the error on the cluster position
        //
        double sigmaSq = 0.0;
        for ( auto it : cluster ) {
          sigmaSq += it.charge()*(it.localPosition().x()-weightedPosX)*(it.localPosition().x()-weightedPosX);
        } 
        double sigma = sqrt(sigmaSq/totalCharge);
        
        Amg::MatrixX* covN = new Amg::MatrixX(1,1);
        (*covN)(0,0) = sigma;

        //
        // memory allocated dynamically for the PrepRawData is managed by Event Store in the converters
        //
        sTgcPrepData* prdN = new sTgcPrepData(clusterId,hash,localPosition,
            rdoList, covN, cluster.at(0).detectorElement());
        clustersVect.push_back(prdN);   
      }
    }
  }

  return StatusCode::SUCCESS;
}


bool Muon::SimpleSTgcClusterBuilderTool::addStrip(Muon::sTgcPrepData& strip)
{

  Identifier prd_id = strip.identify();
  int multilayer = m_stgcIdHelper->multilayer(prd_id);
  int gasGap = m_stgcIdHelper->gasGap(prd_id);
  unsigned int stripNum = m_stgcIdHelper->channel(prd_id);

  // if no cluster is present start creating a new one
  if ( m_clustersStripNum[multilayer][gasGap].size()==0 ) {

    set<unsigned int> clusterStripNum;
    vector<Muon::sTgcPrepData> cluster;

    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    m_clustersStripNum[multilayer][gasGap].push_back(clusterStripNum);
    m_clusters[multilayer][gasGap].push_back(cluster);

    return true;
  }
  else {
    //
    // check if the strip can be added to a cluster
    //
    for ( unsigned int i=0 ; i<m_clustersStripNum[multilayer][gasGap].size() ; ++i  ) {

      unsigned int firstStrip = *(m_clustersStripNum[multilayer][gasGap].at(i).begin());
      unsigned int lastStrip  = *(m_clustersStripNum[multilayer][gasGap].at(i).end());

      if ( stripNum==lastStrip+1 || stripNum==firstStrip-1 ) {

	m_clustersStripNum[multilayer][gasGap].at(i).insert(stripNum);
	m_clusters[multilayer][gasGap].at(i).push_back(strip);

	return true;
      }
    }
    // if not, build a new cluster starting from it
    //
    set<unsigned int> clusterStripNum;
    vector<Muon::sTgcPrepData> cluster;
        
    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    m_clustersStripNum[multilayer][gasGap].push_back(clusterStripNum);
    m_clusters[multilayer][gasGap].push_back(cluster);

    return true;
  }

  return false;
}
