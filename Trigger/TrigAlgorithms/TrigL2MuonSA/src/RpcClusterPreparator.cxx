/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>

#include "RpcClusterPreparator.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "CxxUtils/phihelper.h"

TrigL2MuonSA::RpcClusterPreparator::RpcClusterPreparator(const std::string& type, 
                                                         const std::string& name,
                                                         const IInterface*  parent): 
   AthAlgTool(type,name,parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcClusterPreparator::initialize()
{
   ATH_CHECK(m_idHelperSvc.retrieve());
   ATH_MSG_DEBUG("Retrieved " << m_idHelperSvc);
   ATH_CHECK( m_recRPCRoiTool.retrieve() );


   return StatusCode::SUCCESS; 
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcClusterPreparator::clusteringRPCs(const bool doMultiMuon,
                                                              std::vector<const Muon::RpcPrepDataCollection*> rpcCols, 
                                                              const TrigRoiDescriptor*          p_roids, 
                                                              const ToolHandle<ClusterPatFinder>*     clusterPatFinder, 
                                                              TrigL2MuonSA::RpcLayerClusters&   rpcLayerClusters) const
{
  
  // pattern : map<int, Muon::RpcPrepData*, less<int>>
  std::map<Identifier,pattern> digits;
  digits.clear();

  for( const Muon::RpcPrepDataCollection* theCollection : rpcCols ){

    if(theCollection->size()>0){
      // build the patterns
      if(buildPatterns(doMultiMuon, p_roids, theCollection, digits)){
        buildClusters(clusterPatFinder, digits, rpcLayerClusters);
      }
    }
    //clear map at each collection
    digits.clear();
  }
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

int TrigL2MuonSA::RpcClusterPreparator::buildPatterns(const bool doMultiMuon, 
                                                      const TrigRoiDescriptor*        p_roids, 
                                                      const Muon::RpcPrepDataCollection* rpcCollection,
                                                      std::map<Identifier, pattern>&     digits) const 
{
  ATH_MSG_DEBUG("start building patterns of RPC hits for clustering");
  float deta_thr = 0.1;
  float dphi_thr = 0.1;
  float dynamic_add = 0.02;
  if(doMultiMuon){
    ATH_MSG_DEBUG("# dynamic search window of RPC");
    m_recRPCRoiTool->roiData( p_roids->roiWord() );
    double RoiPhiMin(0);
    double RoiPhiMax(0);
    double RoiEtaMin(0);
    double RoiEtaMax(0);
    m_recRPCRoiTool->RoIsize(p_roids->roiWord(), RoiEtaMin, RoiEtaMax, RoiPhiMin, RoiPhiMax);
    ATH_MSG_DEBUG( "  ... RoI Phi min = " << RoiPhiMin << " RoI Phi max = " << RoiPhiMax << " RoI Eta min = " << RoiEtaMin << " RoI Eta max = " << RoiEtaMax );
    deta_thr = std::abs( RoiEtaMax - RoiEtaMin )/2. + dynamic_add;
    dphi_thr = std::abs( std::acos( std::cos( RoiPhiMax - RoiPhiMin ) ) )/2. + dynamic_add;
    ATH_MSG_DEBUG( "## deta/dphi threshold = " << deta_thr << "/" << dphi_thr);
  }
  
  // here we loop over the digits in the collection and fill the patterns
  Identifier eleId=rpcCollection->identify();
  // loop over digits in collection
  ATH_MSG_DEBUG("# select RPC hits around RoI");
  for( const Muon::RpcPrepData* rpcDigit : *rpcCollection ){
    const Identifier id=rpcDigit->identify();
    const int nstrip        = m_idHelperSvc->rpcIdHelper().strip(id);
    const int doubletZ      = m_idHelperSvc->rpcIdHelper().doubletZ(id);
    const int doubletPhi    = m_idHelperSvc->rpcIdHelper().doubletPhi(id);
    const int gasGap        = m_idHelperSvc->rpcIdHelper().gasGap(id);
    const bool measPhi  = m_idHelperSvc->rpcIdHelper().measuresPhi(id);
    
    //select RPC hits around RoI
    const Amg::Vector3D globalpos = rpcDigit->globalPosition();
    const double hitx=globalpos.x();
    const double hity=globalpos.y();
    const double hitz=globalpos.z();

    const float r2 = hitx*hitx+hity*hity;
    float phi = std::atan2(hity,hitx);
    const float l = std::sqrt(hitz*hitz+r2);
    const float tan = std::sqrt( (l-hitz)/(l+hitz) );
    const float eta = -std::log(tan);
    const float deta = std::abs(p_roids->eta() - eta);
    const float dphi = std::abs(CxxUtils::wrapToPi(p_roids->phi() - phi));

    ATH_MSG_DEBUG( " ... RPC hit deta/dphi frm RoI = " << deta << "/" << dphi << ", x/y/z = " << hitx << "/" << hity << "/" << hitz);

    // use Id of first strip to identify a panel.
    Identifier panelId=m_idHelperSvc->rpcIdHelper().channelID(eleId,doubletZ,doubletPhi,gasGap,measPhi,1);

    if ( deta<deta_thr && dphi<dphi_thr){
      if(digits.find(panelId)==digits.end()){ // first hit on this panel
        pattern newPatt;
        newPatt[nstrip]=rpcDigit;
        digits[panelId]=newPatt;

      } else { // use existing pattern

        if(digits[panelId].find(nstrip)==digits[panelId].end()){ // no hits on this strip before
          digits[panelId][nstrip]=rpcDigit;
        } else if(digits[panelId][nstrip]->time()> rpcDigit->time()){
          digits[panelId][nstrip]=rpcDigit; // if more than one digit, keep only the one with lowest time        
        }
      }
    }
  }

  return digits.size();
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcClusterPreparator::buildClusters(const ToolHandle<ClusterPatFinder>*   clusterPatFinder,
                                                       std::map<Identifier, pattern>&  digits, 
                                                       TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const
{
  // loop over existing patterns

  std::map<Identifier, pattern >::iterator patt_it=digits.begin();

  for(;patt_it!=digits.end();++patt_it){ 

    Identifier panelId=(*patt_it).first; // this is the panel id

    const bool measphi  = m_idHelperSvc->rpcIdHelper().measuresPhi(panelId);
    std::map<int, const Muon::RpcPrepData*, std::less<int> >::iterator dig_it=digits[panelId].begin();

    std::vector<Identifier> theIDs;
    int lastStrip=-999;
    Amg::Vector3D globalPosition(0,0,0);
    float clusterWidth=0;
    unsigned int count=0;
    
    // get the ID from the first digit in collection
    for(;dig_it!=digits[panelId].end();++dig_it){ // loop over patterns

      const MuonGM::RpcReadoutElement* descriptor = (*dig_it).second->detectorElement();

      const int doubletR      = m_idHelperSvc->rpcIdHelper().doubletR((*dig_it).second->identify());
      const int gasGap        = m_idHelperSvc->rpcIdHelper().gasGap((*dig_it).second->identify());
      const int stationEta    = m_idHelperSvc->rpcIdHelper().stationEta((*dig_it).second->identify());
      std::string stationName = m_idHelperSvc->rpcIdHelper().stationNameString(m_idHelperSvc->rpcIdHelper().stationName((*dig_it).second->identify()));

      int layer = 0;
      // BO
      if (stationName.substr(0,2)=="BO") layer = 4;
      // doubletR
      layer += 2*(doubletR-1);
      // BML7 special chamber with 1 RPC doublet (doubletR=1 but RPC2) :
      if (stationName.substr(0,3)=="BML"&&stationEta==7) layer+=2;
      // gasGap
      layer += gasGap - 1;

      if(lastStrip==-999){ // first hit of a cluster..
        lastStrip=(*dig_it).first;
        theIDs.push_back((*dig_it).second->identify());
        globalPosition+=descriptor->stripPos((*dig_it).second->identify());
        clusterWidth+=descriptor->StripWidth(measphi);
        ATH_MSG_DEBUG( ">> first hit of a cluster, id = " << lastStrip << ", RPChit x/y/z/ = " << descriptor->stripPos((*dig_it).second->identify()).x() << "/" << descriptor->stripPos((*dig_it).second->identify()).y() << "/" << descriptor->stripPos((*dig_it).second->identify()).z());

      } else if(std::abs(lastStrip-(*dig_it).first)==1){ // still on the same cluster

        lastStrip=(*dig_it).first;
        theIDs.push_back((*dig_it).second->identify());
        globalPosition+=descriptor->stripPos((*dig_it).second->identify());
        clusterWidth+=descriptor->StripWidth(measphi);
        ATH_MSG_DEBUG( " ... still on the same cluster,  id = " << lastStrip << ", RPChit x/y/z/ = " << descriptor->stripPos((*dig_it).second->identify()).x() << "/" << descriptor->stripPos((*dig_it).second->identify()).y() << "/" << descriptor->stripPos((*dig_it).second->identify()).z());

      } else { // close the cluster

        globalPosition=globalPosition*(1/(float)theIDs.size());

        ATH_MSG_DEBUG(" ... close the cluster ---> cluster at " << stationName << ", x/y/z = " << globalPosition.x() << "/" << globalPosition.y() << "/" << globalPosition.z());
        (*clusterPatFinder)->addCluster(stationName, stationEta, measphi, gasGap, doubletR, globalPosition.x(), globalPosition.y(), globalPosition.z(), rpcLayerClusters);

        // clear all the numbers and restart a new cluster
        lastStrip=(*dig_it).first;
        theIDs.clear();
      
        theIDs.push_back((*dig_it).second->identify());
        globalPosition=descriptor->stripPos((*dig_it).second->identify());
        clusterWidth=descriptor->StripWidth(measphi);
      }


      // if we are at the end, close the custer anyway and fill it with what we have found

      if(count==digits[panelId].size()-1&&theIDs.size()>0){
        globalPosition=globalPosition*(1/(float)theIDs.size());
        
        //addCluster to PatFinder
        ATH_MSG_DEBUG(" ... close the cluster ---> cluster at " << stationName << ", x/y/z = " << globalPosition.x() << "/" << globalPosition.y() << "/" << globalPosition.z());
        
        (*clusterPatFinder)->addCluster(stationName, stationEta, measphi, gasGap, doubletR, globalPosition.x(), globalPosition.y(), globalPosition.z(), rpcLayerClusters);
      }
      count++;
    }
  }

}

