/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  ATH_CHECK(detStore()->retrieve(m_muonMgr));  
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

  //
  // define the identifier hash
  IdentifierHash hash;

  double resolution=0.;
  bool isWire = false;
  if ( stripsVect.size()>0 ) {
    resolution = stripsVect.at(0).localCovariance()(0,0);
    Identifier chanId = stripsVect.at(0).identify();
    if ( m_stgcIdHelper->channelType(chanId)==2 ) isWire = true;
    ATH_MSG_DEBUG("isWire: " << isWire << "Single channel resolution: " << resolution);
  }
  else {
    ATH_MSG_DEBUG("Size of the channel vectors is zero");
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

  // now add the clusters to the PRD container
  //
  clustersVect.clear();

  for ( unsigned int multilayer =0 ; multilayer<3 ; ++multilayer ) {
    for ( unsigned int gasGap=0 ; gasGap<5 ; ++gasGap ) {
      //
      // loop on the clusters of that gap
      //
      for ( unsigned int i=0 ; i<m_clusters[multilayer][gasGap].size() ; ++i ) { 
        // get the cluster
        std::vector<Muon::sTgcPrepData> const &cluster = m_clusters[multilayer][gasGap].at(i);
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
          double weight = 0.0;
          isWire ? weight = 1.0 : weight = it.charge(); 
          ATH_MSG_DEBUG("isWire: " << isWire << " weight: " << weight);
          weightedPosX += it.localPosition().x()*weight;
          totalCharge += weight;
          ATH_MSG_DEBUG("Channel local position and charge: " << it.localPosition().x() << " " << it.charge() );
          //
          // Set the cluster identifier to the max charge strip
          //
          if ( isWire ) {
            clusterId = it.identify();
          }
          if ( !isWire && it.charge()>maxCharge ) {
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
        ATH_MSG_DEBUG("Cluster size: " << cluster.size());
        if ( cluster.size() > 1 ) {
          double weight = 0.0;
          for ( auto it : cluster ) {
            isWire ? weight = 1.0 : weight = it.charge(); 
            ATH_MSG_DEBUG("isWire: " << isWire << " weight: " << weight);
            //sigmaSq += weight*(it.localPosition().x()-weightedPosX)*(it.localPosition().x()-weightedPosX);
            sigmaSq += weight*weight*resolution;
            ATH_MSG_DEBUG(">>>> posX: " << it.localPosition().x() << " weightedPosX: " << weightedPosX); 
          } 
        }
        else {
          sigmaSq = resolution;
        }
        sigmaSq = sigmaSq/(totalCharge*totalCharge*12);
        ATH_MSG_DEBUG("Uncertainty on cluster position is: " << sqrt(sigmaSq));         
        Amg::MatrixX* covN = new Amg::MatrixX(1,1);
        (*covN)(0,0) = sigmaSq;

        //
        // memory allocated dynamically for the PrepRawData is managed by Event Store in the converters
        //
        sTgcPrepData* prdN = new sTgcPrepData(clusterId,hash,localPosition,
            rdoList, covN, cluster.at(0).detectorElement());
        clustersVect.push_back(prdN);   
      }
    }
  }

  ATH_MSG_DEBUG("Size of the output cluster vector: " << clustersVect.size());
  return StatusCode::SUCCESS;
}


bool Muon::SimpleSTgcClusterBuilderTool::addStrip(Muon::sTgcPrepData& strip)
{

  Identifier prd_id = strip.identify();
  int channelType = m_stgcIdHelper->channelType(prd_id);
  int multilayer = m_stgcIdHelper->multilayer(prd_id);
  int gasGap = m_stgcIdHelper->gasGap(prd_id);
  unsigned int stripNum = m_stgcIdHelper->channel(prd_id);

  ATH_MSG_DEBUG(">>>>>>>>>>>>>> In addStrip: channelType, multilayer, gasGap, stripNum: " << channelType 
      << " " << multilayer << " " 
      << gasGap << " " << stripNum);
  
  // if no cluster is present start creating a new one
  if ( m_clustersStripNum[multilayer][gasGap].size()==0 ) {

    ATH_MSG_DEBUG( ">>> No strip present in this gap: adding it as first cluster " );
    set<unsigned int> clusterStripNum;
    vector<Muon::sTgcPrepData> cluster;

    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    std::vector<std::set<unsigned int>> &clustersStripNum = m_clustersStripNum[multilayer][gasGap];
    std::vector<std::vector<Muon::sTgcPrepData>> &clusters = m_clusters[multilayer][gasGap];

    clustersStripNum.emplace_back();
    clustersStripNum.back().insert(stripNum);
    clusters.emplace_back();
    clusters.back().push_back(strip);

    return true;
  }
  else {
    //
    // check if the strip can be added to a cluster
    //
    for ( unsigned int i=0 ; i<m_clustersStripNum[multilayer][gasGap].size() ; ++i  ) {

      set<unsigned int> &clusterStripNum = m_clustersStripNum[multilayer][gasGap].at(i);

      unsigned int firstStrip = *(clusterStripNum.begin());
      unsigned int lastStrip  = *(--clusterStripNum.end());

      ATH_MSG_DEBUG("First strip and last strip are: " << firstStrip << " " << lastStrip);
      if ( stripNum==lastStrip+1 || stripNum==firstStrip-1 ) {

        ATH_MSG_DEBUG(">> inserting a new strip");
	m_clustersStripNum[multilayer][gasGap].at(i).insert(stripNum);
	m_clusters[multilayer][gasGap].at(i).push_back(strip);

        ATH_MSG_DEBUG("size after inserting is: " << m_clustersStripNum[multilayer][gasGap].at(i).size());
        ATH_MSG_DEBUG("and the first and last strip are: " 
          << *(m_clustersStripNum[multilayer][gasGap].at(i).begin()) << " "  
          << *(--m_clustersStripNum[multilayer][gasGap].at(i).end())); 
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
