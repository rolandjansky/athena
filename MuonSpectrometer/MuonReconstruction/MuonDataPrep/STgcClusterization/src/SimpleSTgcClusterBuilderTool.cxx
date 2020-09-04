/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleSTgcClusterBuilderTool.h"
#include "MuonPrepRawData/sTgcPrepData.h"

#include <set>
#include <vector>

using namespace Muon;

Muon::SimpleSTgcClusterBuilderTool::SimpleSTgcClusterBuilderTool(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareProperty("ChargeCut", m_chargeCut=0.0);
  declareProperty("maxHoleSize",m_maxHoleSize=1);
  declareProperty("addError",m_addError=0);
}

StatusCode Muon::SimpleSTgcClusterBuilderTool::initialize()
{
  ATH_CHECK( m_idHelperSvc.retrieve() );
  return StatusCode::SUCCESS;
}

//
// Build the clusters given a vector of single-hit PRD
//
StatusCode Muon::SimpleSTgcClusterBuilderTool::getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, 
							   std::vector<Muon::sTgcPrepData*>& clustersVect) const
{

  ATH_MSG_DEBUG("Size of the input vector: " << stripsVect.size()); 

  //
  // define the identifier hash
  IdentifierHash hash;

  double resolution=0.;
  bool isStrip = false;
  if ( stripsVect.size()>0 ) {
    resolution = stripsVect.at(0).localCovariance()(0,0);
    Identifier chanId = stripsVect.at(0).identify();
    if ( m_idHelperSvc->stgcIdHelper().channelType(chanId)==1 ) isStrip = true;
    ATH_MSG_DEBUG(" channelType " << m_idHelperSvc->stgcIdHelper().channelType(chanId));
    ATH_MSG_DEBUG("isStrip: " << isStrip << "Single channel resolution: " << resolution);
  }
  else {
    ATH_MSG_DEBUG("Size of the channel vectors is zero");
    return StatusCode::SUCCESS;
  }

  std::vector<std::set<unsigned int>> clustersStripNum[2][4];
  std::vector<std::vector<Muon::sTgcPrepData>> clusters[2][4];

  for ( auto& it : stripsVect ) {
    if ( !addStrip(it,clustersStripNum,clusters) ) {
      ATH_MSG_ERROR("Could not add a strip to the sTGC clusters");
      return StatusCode::FAILURE;
    }
  } 

  // now add the clusters to the PRD container
  //
  clustersVect.clear();

  for ( unsigned int multilayer=0 ; multilayer<2 ; ++multilayer ) {
    for ( unsigned int gasGap=0 ; gasGap<4 ; ++gasGap ) {
      //
      // loop on the clusters of that gap
      //
      for ( unsigned int i=0 ; i<clusters[multilayer][gasGap].size() ; ++i ) { 
        // get the cluster
        std::vector<Muon::sTgcPrepData> const &cluster = clusters[multilayer][gasGap].at(i);
        //
        // loop on the strips and set the cluster weighted position and charge
        //
        std::vector<Identifier> rdoList;
        //vectors to hold the properties of the elements of a cluster
        std::vector<int> elementsCharge;
        std::vector<short int> elementsTime;
        std::vector<uint16_t> elementsChannel;
        Identifier clusterId;
        double weightedPosX = 0.0;
        double posY = (cluster.at(0)).localPosition().y();

        double maxCharge = -1.0;
        double totalCharge  = 0.0;
        for ( const auto& it : cluster ) {
          rdoList.push_back(it.identify());
          elementsCharge.push_back(it.charge());
          elementsChannel.push_back(m_idHelperSvc->stgcIdHelper().channel(it.identify()));
          elementsTime.push_back(it.time());
          double weight = 0.0;
          isStrip ? weight = it.charge() : weight = 1.0; 
          ATH_MSG_DEBUG("isStrip: " << isStrip << " weight: " << weight);
          weightedPosX += it.localPosition().x()*weight;
          totalCharge += weight;
          ATH_MSG_DEBUG("Channel local position and charge: " << it.localPosition().x() << " " 
			<< it.charge() );
          //
          // Set the cluster identifier to the max charge strip
          //
          if ( isStrip && it.charge()>maxCharge ) {
            maxCharge = it.charge();
            clusterId = it.identify();
          }
          if ( !isStrip) {
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
          for ( const auto& it : cluster ) {
            isStrip ? weight = it.charge() : weight = 1.0; 
            ATH_MSG_DEBUG("isStrip: " << isStrip << " weight: " << weight);
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
        (*covN)(0,0) = sigmaSq + m_addError*m_addError;

        //
        // memory allocated dynamically for the PrepRawData is managed by Event Store in the converters
        //
        ATH_MSG_DEBUG("error on cluster " << sqrt((*covN)(0,0)) << " added error " <<  m_addError); 
        
        sTgcPrepData* prdN = new sTgcPrepData(clusterId,hash,localPosition,
            rdoList, covN, cluster.at(0).detectorElement(),
            std::accumulate(elementsCharge.begin(),elementsCharge.end(),0),(short int)0,(uint16_t) 0,elementsChannel,elementsTime,elementsCharge);
        clustersVect.push_back(prdN);   
      }
    }
  }

  ATH_MSG_DEBUG("Size of the output cluster vector: " << clustersVect.size());

  return StatusCode::SUCCESS;
}


bool Muon::SimpleSTgcClusterBuilderTool::addStrip(const Muon::sTgcPrepData& strip, std::vector<std::set<unsigned int>> clustersStripNum[2][4], 
                  std::vector<std::vector<Muon::sTgcPrepData>> clusters[2][4])const
{

  Identifier prd_id = strip.identify();
  int channelType = m_idHelperSvc->stgcIdHelper().channelType(prd_id);
  int multilayer = m_idHelperSvc->stgcIdHelper().multilayer(prd_id)-1;
  int gasGap = m_idHelperSvc->stgcIdHelper().gasGap(prd_id)-1;
  unsigned int stripNum = m_idHelperSvc->stgcIdHelper().channel(prd_id);

  ATH_MSG_DEBUG(">>>>>>>>>>>>>> In addStrip: channelType, multilayer, gasGap, stripNum: " << channelType 
      << " " << multilayer << " " 
      << gasGap << " " << stripNum);
  
  // if no cluster is present start creating a new one
  if (clustersStripNum[multilayer][gasGap].size()==0 ) {

    ATH_MSG_DEBUG( ">>> No strip present in this gap: adding it as first cluster " );
    std::set<unsigned int> clusterStripNum;
    std::vector<Muon::sTgcPrepData> cluster;

    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    std::vector<std::set<unsigned int>> &clustersOfLayerStripNum = clustersStripNum[multilayer][gasGap];
    std::vector<std::vector<Muon::sTgcPrepData>> &clustersOfLayer = clusters[multilayer][gasGap];

    clustersOfLayerStripNum.emplace_back();
    clustersOfLayerStripNum.back().insert(stripNum);
    clustersOfLayer.emplace_back();
    clustersOfLayer.back().push_back(strip);

    return true;
  }
  else {

    //
    // check if the strip can be added to a cluster
    //
    for ( unsigned int i=0 ; i<clustersStripNum[multilayer][gasGap].size() ; ++i  ) {

      std::set<unsigned int> &clusterStripNum = clustersStripNum[multilayer][gasGap].at(i);

      unsigned int firstStrip = *(clusterStripNum.begin());
      unsigned int lastStrip  = *(--clusterStripNum.end());
      ATH_MSG_DEBUG("First strip and last strip are: " << firstStrip << " " << lastStrip);
      unsigned int diffFirst = (stripNum-firstStrip) > 0 ? stripNum - firstStrip : firstStrip-stripNum ;
      unsigned int diffLast  = (stripNum-lastStrip)  > 0 ? stripNum - lastStrip  : lastStrip-stripNum ;
      if ( diffFirst<=m_maxHoleSize + 1 || diffLast<=m_maxHoleSize + 1) {

        ATH_MSG_DEBUG(">> inserting a new strip");
	      clustersStripNum[multilayer][gasGap].at(i).insert(stripNum);
	      clusters[multilayer][gasGap].at(i).push_back(strip);

        ATH_MSG_DEBUG("size after inserting is: " << clustersStripNum[multilayer][gasGap].at(i).size());
        ATH_MSG_DEBUG("and the first and last strip are: " 
          << *(clustersStripNum[multilayer][gasGap].at(i).begin()) << " "  
          << *(--clustersStripNum[multilayer][gasGap].at(i).end())); 
	return true;
      }
    }
    // if not, build a new cluster starting from it
    //
    std::set<unsigned int> clusterStripNum;
    std::vector<Muon::sTgcPrepData> cluster;
        
    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    clustersStripNum[multilayer][gasGap].push_back(clusterStripNum);
    clusters[multilayer][gasGap].push_back(cluster);

    return true;
  }

  return false;
}

///
/// sort the strips if needed
void SimpleSTgcClusterBuilderTool::dumpStrips( std::vector<Muon::sTgcPrepData>& stripsVect,
					       std::vector<Muon::sTgcPrepData*>& clustersVect ) const 
{

  ATH_MSG_INFO("====> Dumping all strips:  ");
  for ( const auto& it : stripsVect ) {
    Identifier stripId = it.identify(); 
    ATH_MSG_INFO("Strip identifier: " << m_idHelperSvc->stgcIdHelper().show_to_string(stripId) ); 
  }

  ATH_MSG_INFO("Dumping all clusters:  ");
  for ( auto it : clustersVect ) {
    Identifier clusterId = it->identify(); 
    ATH_MSG_INFO("***> New cluster identifier: " << m_idHelperSvc->stgcIdHelper().show_to_string(clusterId) ); 
    ATH_MSG_INFO("Cluster size: " << it->rdoList().size() );
    ATH_MSG_INFO("List of associated RDO's: ");

  }

  return;
}
