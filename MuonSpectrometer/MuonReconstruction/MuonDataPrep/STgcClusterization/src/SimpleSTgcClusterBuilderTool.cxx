/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleSTgcClusterBuilderTool.h"
#include "MuonPrepRawData/sTgcPrepData.h"

#include <set>
#include <vector>

using namespace Muon;

//============================================================================
Muon::SimpleSTgcClusterBuilderTool::SimpleSTgcClusterBuilderTool(const std::string& t, const std::string& n, const IInterface* p) 
: AthAlgTool(t,n,p) {
    declareProperty("ChargeCut", m_chargeCut=0.0);
    declareProperty("maxHoleSize",m_maxHoleSize=1);
    declareProperty("addError",m_addError=0);
}


//============================================================================
StatusCode Muon::SimpleSTgcClusterBuilderTool::initialize() {
    ATH_CHECK( m_idHelperSvc.retrieve() );
    return StatusCode::SUCCESS;
}


//============================================================================
// Build the clusters given a vector of single-hit PRD
StatusCode Muon::SimpleSTgcClusterBuilderTool::getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, 
							   std::vector<Muon::sTgcPrepData*>& clustersVect) const {

    ATH_MSG_DEBUG("Size of the input vector: " << stripsVect.size()); 

    if (stripsVect.empty()) return StatusCode::SUCCESS;

    // define the identifier hash
    IdentifierHash hash;
  
    Identifier chanId = stripsVect.at(0).identify();
    double resolution = stripsVect.at(0).localCovariance()(0,0);
    bool isStrip = ( m_idHelperSvc->stgcIdHelper().channelType(chanId) == sTgcIdHelper::Strip );
    ATH_MSG_DEBUG(" channelType " << m_idHelperSvc->stgcIdHelper().channelType(chanId));
    ATH_MSG_DEBUG(" isStrip: " << isStrip << "Single channel resolution: " << resolution);

    std::vector<std::set<unsigned int>> clustersStripNum[2][4];
    std::vector<std::vector<Muon::sTgcPrepData>> clusters[2][4];

    for ( auto& it : stripsVect ) {
        if ( !addStrip(it,clustersStripNum,clusters) ) {
            ATH_MSG_ERROR("Could not add a strip to the sTGC clusters");
            return StatusCode::FAILURE;
        }
    } 

    // now add the clusters to the PRD container
    clustersVect.clear();

    for ( unsigned int multilayer{0} ; multilayer<2 ; ++multilayer ) {
        for ( unsigned int gasGap{0} ; gasGap<4 ; ++gasGap ) {
 
            // loop on the clusters of that gap
            for ( unsigned int iclus{0} ; iclus < clusters[multilayer][gasGap].size() ; ++iclus ) { 
 
                std::vector<Muon::sTgcPrepData> const &cluster = clusters[multilayer][gasGap][iclus];
 
                //vectors to hold the properties of the elements of a cluster
                std::vector<Identifier> rdoList;
                std::vector<int>        elementsCharge;
                std::vector<short int>  elementsTime;
                std::vector<uint16_t>   elementsChannel;

                Identifier clusterId;
                double posY = cluster[0].localPosition().y();
                double weightedPosX{0.0};
                double maxCharge{-1.0};
                double sumWeight{0.0};

                // cluster weighted position and charge
                for (const auto& prd : cluster ) {

                    rdoList.push_back(prd.identify());
                    elementsCharge.push_back(prd.charge());
                    elementsChannel.push_back(m_idHelperSvc->stgcIdHelper().channel(prd.identify()));
                    elementsTime.push_back(prd.time());
          
                    double weight = isStrip ? prd.charge() : 1.0;
                    ATH_MSG_DEBUG("isStrip: " << isStrip << " weight: " << weight);

                    weightedPosX += prd.localPosition().x()*weight;
                    sumWeight    += weight;
                    ATH_MSG_DEBUG("Channel local position and charge: " << prd.localPosition().x() << " " << prd.charge() );

                    // Set the cluster identifier to the max charge strip
                    if ( !isStrip) {
                        clusterId = prd.identify();
                    } else if ( prd.charge()>maxCharge ) {
                        maxCharge = prd.charge();
                        clusterId = prd.identify();
                    }
                }
 
                weightedPosX /= sumWeight;
                Amg::Vector2D localPosition(weightedPosX, posY);
        
                // get the error on the cluster position
                double sigmaSq{0.0};
                ATH_MSG_DEBUG("Cluster size: " << cluster.size());
                if ( cluster.size() > 1 ) {
                    double weight = 0.0;
                    for ( const auto& prd : cluster ) {
                        isStrip ? weight = prd.charge() : weight = 1.0; 
                        ATH_MSG_DEBUG("isStrip: " << isStrip << " weight: " << weight);
                        //sigmaSq += weight*(prd.localPosition().x()-weightedPosX)*(prd.localPosition().x()-weightedPosX);
                        sigmaSq += weight*weight*resolution;
                        ATH_MSG_DEBUG(">>>> posX: " << prd.localPosition().x() << " weightedPosX: " << weightedPosX); 
                    } 
                } else {
                    sigmaSq = resolution;
                }

                sigmaSq  /= sumWeight*sumWeight;
                ATH_MSG_DEBUG("Uncertainty on cluster position is: " << sqrt(sigmaSq));         
                auto covN = Amg::MatrixX(1,1);
                covN(0,0) = sigmaSq + m_addError*m_addError;

                // memory allocated dynamically for the PrepRawData is managed by Event Store in the converters
                ATH_MSG_DEBUG("error on cluster " << sqrt((covN)(0,0)) << " added error " <<  m_addError);

                sTgcPrepData* prdN = new sTgcPrepData(
                  clusterId,
                  hash,
                  localPosition,
                  rdoList,
                  covN,
                  cluster.at(0).detectorElement(),
                  std::accumulate(elementsCharge.begin(), elementsCharge.end(), 0),
                  (short int)0,
                  (uint16_t)0,
                  elementsChannel,
                  elementsTime,
                  elementsCharge);
                
                clustersVect.push_back(prdN);
            }
        }
    }

    ATH_MSG_DEBUG("Size of the output cluster vector: " << clustersVect.size());

    return StatusCode::SUCCESS;
}


//============================================================================
bool Muon::SimpleSTgcClusterBuilderTool::addStrip(const Muon::sTgcPrepData& strip, 
                       std::vector<std::set<unsigned int>> clustersStripNum[2][4], 
                       std::vector<std::vector<Muon::sTgcPrepData>> clusters[2][4]) const {
    Identifier     prd_id = strip.identify();
    int       channelType = m_idHelperSvc->stgcIdHelper().channelType(prd_id);
    int        multilayer = m_idHelperSvc->stgcIdHelper().multilayer(prd_id)-1;
    int            gasGap = m_idHelperSvc->stgcIdHelper().gasGap(prd_id)-1;
    unsigned int stripNum = m_idHelperSvc->stgcIdHelper().channel(prd_id);

    ATH_MSG_DEBUG(">>>> In addStrip: channelType, multilayer, gasGap, channelNum: " << channelType << " " << multilayer << " " << gasGap << " " << stripNum);
    
    // Try to add the PRD to an existing cluster
    size_t nclusters = clustersStripNum[multilayer][gasGap].size();
    for ( unsigned int iclus{0} ; iclus < nclusters ; ++iclus  ) {

        std::set<unsigned int>&        clusStrips = clustersStripNum[multilayer][gasGap][iclus];
        std::vector<Muon::sTgcPrepData>& clusPRDs = clusters[multilayer][gasGap][iclus];

        unsigned int firstStrip = *(clusStrips.begin());
        unsigned int  lastStrip = *(--clusStrips.end());
        unsigned int  diffFirst = stripNum > firstStrip ? stripNum - firstStrip : firstStrip - stripNum ;
        unsigned int   diffLast = stripNum > lastStrip  ? stripNum - lastStrip  : lastStrip - stripNum ;
        if ( diffFirst <= m_maxHoleSize + 1 || diffLast <= m_maxHoleSize + 1 ) { // channels come in order
            clusStrips.insert(stripNum);
            clusPRDs.push_back(strip);
      
            ATH_MSG_DEBUG(">> Added channel: "<<stripNum <<" - new cluster size: " << clusStrips.size() 
                       << " - channel range: [" << *(clusStrips.begin()) << ", "  << *(--clusStrips.end()) << "]");

            return true;
        }
    }

    // If not added yet, build a new cluster starting from this channel
    ATH_MSG_DEBUG(">> Channel added as a new cluster");
    clustersStripNum[multilayer][gasGap].push_back({stripNum});
    clusters[multilayer][gasGap].push_back({strip});
    return true;
}


//============================================================================
/// sort the strips if needed
void SimpleSTgcClusterBuilderTool::dumpStrips( std::vector<Muon::sTgcPrepData>& stripsVect, std::vector<Muon::sTgcPrepData*>& clustersVect ) const {
    ATH_MSG_INFO("====> Dumping all strips:  ");
    for ( const auto& it : stripsVect ) {
        Identifier stripId = it.identify(); 
        ATH_MSG_INFO("Strip identifier: " << m_idHelperSvc->stgcIdHelper().show_to_string(stripId) ); 
    }

    ATH_MSG_INFO("Dumping all clusters:  ");
    for ( auto *it : clustersVect ) {
        Identifier clusterId = it->identify(); 
        ATH_MSG_INFO("***> New cluster identifier: " << m_idHelperSvc->stgcIdHelper().show_to_string(clusterId) ); 
        ATH_MSG_INFO("Cluster size: " << it->rdoList().size() );
        ATH_MSG_INFO("List of associated RDO's: ");
    }
}
