/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "RpcData.h"
#include "RecMuonRoIUtils.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "CxxUtils/phihelper.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcDataPreparator::RpcDataPreparator(const std::string& type, 
                                                   const std::string& name,
                                                   const IInterface*  parent): 
   AthAlgTool(type,name,parent) {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::initialize()
{

   // Locate RegionSelector
   ATH_CHECK( m_regionSelector.retrieve() );

   ATH_CHECK( m_recRPCRoiTool.retrieve() );

   ATH_CHECK(m_idHelperSvc.retrieve());
   ATH_MSG_DEBUG("Retrieved " << m_idHelperSvc);

   ATH_CHECK(m_rpcPrepContainerKey.initialize());

   ATH_CHECK(m_clusterPreparator.retrieve());
   ATH_MSG_DEBUG("Retrieved service " << m_clusterPreparator);

   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcDataPreparator::setRoIBasedDataAccess(bool use_RoIBasedDataAccess)
{
  m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcDataPreparator::setMultiMuonTrigger( const bool multiMuonTrigger )
{
  m_doMultiMuon = multiMuonTrigger;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::prepareData(const TrigRoiDescriptor*    p_roids,
                                                        TrigL2MuonSA::RpcHits&      rpcHits,
                                                        TrigL2MuonSA::RpcLayerHits& rpcLayerHits,
                                                        const ToolHandle<RpcPatFinder>*   rpcPatFinder) const
{
  // RPC data extraction referring TrigMuonEFStandaloneTrackTool and MuonHoughPatternFinderTool
  rpcHits.clear();

  if( m_emulateNoRpcHit )
    return StatusCode::SUCCESS;

   const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;

   std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
   std::vector<IdentifierHash> rpcHashList;
   std::vector<IdentifierHash> rpcHashListWithData;
   std::vector<IdentifierHash> rpcHashList_cache;

   if (m_use_RoIBasedDataAccess) {

     ATH_MSG_DEBUG("Use RoI based data access");
     
     if (iroi) m_regionSelector->HashIDList(*iroi, rpcHashList);
     else {
       TrigRoiDescriptor fullscan_roi( true );
       m_regionSelector->HashIDList(fullscan_roi, rpcHashList);
     }
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());

     std::vector<uint32_t> rpcRobList;
     m_regionSelector->ROBIDList(*iroi, rpcRobList);
   } else {
     
     ATH_MSG_DEBUG("Use full data access");
     
     TrigRoiDescriptor fullscan_roi( true );
     m_regionSelector->HashIDList(fullscan_roi, rpcHashList);
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());
     
     std::vector<uint32_t> rpcRobList;
     m_regionSelector->ROBIDList(fullscan_roi, rpcRobList);
     
   }
   
   if (!rpcHashList.empty()) {
     
     // Get RPC container
     const Muon::RpcPrepDataContainer* rpcPrds;
     auto rpcPrepContainerHandle = SG::makeHandle(m_rpcPrepContainerKey);
     rpcPrds = rpcPrepContainerHandle.cptr();
     if (!rpcPrepContainerHandle.isValid()) {
       ATH_MSG_ERROR("Cannot retrieve RPC PRD Container key: " << m_rpcPrepContainerKey.key());
       return StatusCode::FAILURE;
     } else {
       ATH_MSG_DEBUG("RPC PRD Container retrieved with key: " << m_rpcPrepContainerKey.key());
     }

     // Get RPC collections
     for(const IdentifierHash& id : rpcHashList) {
       auto RPCcoll = rpcPrds->indexFindPtr(id);

       if( RPCcoll == nullptr ) {
         continue;
       }

       if( RPCcoll->size() == 0) {
         ATH_MSG_DEBUG("Empty RPC list");
         continue;
       }

       rpcHashList_cache.push_back(id);
       rpcCols.push_back(RPCcoll);
     }
   }

   for( const Muon::RpcPrepDataCollection* rpc : rpcCols ){

     rpcHits.reserve( rpcHits.size() + rpc->size() );
     for( const Muon::RpcPrepData* prd : *rpc ) {

       const Identifier id = prd->identify();

       const int doubletR      = m_idHelperSvc->rpcIdHelper().doubletR(id);
       const int doubletPhi    = m_idHelperSvc->rpcIdHelper().doubletPhi(id);
       const int doubletZ      = m_idHelperSvc->rpcIdHelper().doubletZ(id);
       const int gasGap        = m_idHelperSvc->rpcIdHelper().gasGap(id);
       const bool measuresPhi  = m_idHelperSvc->rpcIdHelper().measuresPhi(id);
       const int stationEta    = m_idHelperSvc->rpcIdHelper().stationEta(id);
       std::string stationName = m_idHelperSvc->rpcIdHelper().stationNameString(m_idHelperSvc->rpcIdHelper().stationName(id));

       int layer = 0;
       // BO
       if (stationName.substr(0,2)=="BO") layer = 4;
       // doubletR
       layer += 2*(doubletR-1);
       // BML7 special chamber with 1 RPC doublet (doubletR=1 but RPC2) :
       if (stationName.substr(0,3)=="BML"&&stationEta==7) layer+=2;
       // gasGap
       layer += gasGap - 1;

       const Amg::Vector3D globalpos = prd->globalPosition();
       const double hitx=globalpos.x();
       const double hity=globalpos.y();
       const double hitz=globalpos.z();

       const double hittime = prd->time();
       const MuonGM::RpcReadoutElement* detEl = prd->detectorElement();
       const double distToPhiReadout = detEl->distanceToPhiReadout(globalpos);
       const double distToEtaReadout = detEl->distanceToEtaReadout(globalpos);

       ATH_MSG_DEBUG("Selected Rpc Collection: station name:" << stationName
		     << " global positions x/y/z=" << hitx << "/" << hity << "/" << hitz
		     << " doubletR: " << doubletR << " doubletZ: " << doubletZ << " doubletPhi " << doubletPhi
		     << " gasGap " << gasGap << " layer " << layer << " time " << hittime
		     << " distToEtaReadout " << distToEtaReadout << " distToPhiReadout " << distToPhiReadout);
       
       TrigL2MuonSA::RpcHitData lutDigit;
       
       lutDigit.x           = hitx;
       lutDigit.y           = hity;
       lutDigit.z           = hitz;
       lutDigit.time        = hittime;
       lutDigit.distToEtaReadout = distToEtaReadout;
       lutDigit.distToPhiReadout = distToPhiReadout;
       lutDigit.gasGap      = gasGap;
       lutDigit.doubletR    = doubletR;
       lutDigit.doubletPhi  = doubletPhi;
       lutDigit.doubletZ    = doubletZ;
       lutDigit.measuresPhi = measuresPhi;
       lutDigit.stationName = stationName;
       lutDigit.layer       = layer;
       
       const float r2 = hitx*hitx+hity*hity;
       float phi = std::atan2(hity,hitx);
       const float l = std::sqrt(hitz*hitz+r2);
       const float tan = std::sqrt( (l-hitz)/(l+hitz) );
       const float eta = -std::log(tan);
       const float deta = std::abs(p_roids->eta() - eta);
       const float dphi = std::abs(CxxUtils::wrapToPi(p_roids->phi() - phi));

       lutDigit.eta = eta;
       lutDigit.phi = phi;
       lutDigit.l = l;
       rpcHits.push_back(lutDigit);

       float deta_thr = 0.1;
       float dphi_thr = 0.1;
       float dynamic_add = 0.02;

       //Determine deta, dphi threshold in case of dynamicDeltaRpcMode
       if( m_doMultiMuon ){
         ATH_MSG_DEBUG("Collected RPC hits by MultiMuonTriggerMode");
         m_recRPCRoiTool->roiData( p_roids->roiWord() );
         double RoiPhiMin(0);
         double RoiPhiMax(0);
         double RoiEtaMin(0);
         double RoiEtaMax(0);
         m_recRPCRoiTool->RoIsize(p_roids->roiWord(), RoiEtaMin, RoiEtaMax, RoiPhiMin, RoiPhiMax);
         ATH_MSG_DEBUG( "RoI Phi min = " << RoiPhiMin << " RoI Phi max = " << RoiPhiMax << " RoI Eta min = " << RoiEtaMin << " RoI Eta max = " << RoiEtaMax );
         deta_thr = std::abs( RoiEtaMax - RoiEtaMin )/2. + dynamic_add;
         dphi_thr = std::abs( std::acos( std::cos( RoiPhiMax - RoiPhiMin ) ) )/2. + dynamic_add;
         ATH_MSG_DEBUG( "deta threshold = " << deta_thr);
         ATH_MSG_DEBUG( "dphi threshold = " << dphi_thr);
       }

       if (m_use_RoIBasedDataAccess) {
         if ( deta<deta_thr && dphi<dphi_thr)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz, rpcLayerHits);
       } else {
         if ( deta<0.15 && dphi<0.1)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz, rpcLayerHits);
       }
     }
   }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::prepareData(const TrigRoiDescriptor*         p_roids,
                                                        TrigL2MuonSA::RpcLayerClusters&  rpcLayerClusters,
                                                        const ToolHandle<ClusterPatFinder>*    clusterPatFinder) const
{
  // RPC data extraction referring TrigMuonEFStandaloneTrackTool and MuonHoughPatternFinderTool

  if( m_emulateNoRpcHit )
    return StatusCode::SUCCESS;

   const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;

   std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
   std::vector<IdentifierHash> rpcHashList;
   std::vector<IdentifierHash> rpcHashList_cache;

   if (m_use_RoIBasedDataAccess) {

     ATH_MSG_DEBUG("Use RoI based data access");
     
     if (iroi) m_regionSelector->HashIDList(*iroi, rpcHashList);
     else {
       TrigRoiDescriptor fullscan_roi( true );
       m_regionSelector->HashIDList(fullscan_roi, rpcHashList);
     }
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());
     
   } else {
     
     ATH_MSG_DEBUG("Use full data access");
     
     TrigRoiDescriptor fullscan_roi( true );
     m_regionSelector->HashIDList(fullscan_roi, rpcHashList);
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());
     
   }
   
   if (!rpcHashList.empty()) {
     
     // Get RPC container
     const Muon::RpcPrepDataContainer* rpcPrds;
     auto rpcPrepContainerHandle = SG::makeHandle(m_rpcPrepContainerKey);
     rpcPrds = rpcPrepContainerHandle.cptr();
     if (!rpcPrepContainerHandle.isValid()) {
       ATH_MSG_ERROR("Cannot retrieve RPC PRD Container key: " << m_rpcPrepContainerKey.key());
       return StatusCode::FAILURE;
     } else {
       ATH_MSG_DEBUG("RPC PRD Container retrieved with key: " << m_rpcPrepContainerKey.key());
     }

     // Get RPC collections
     for(const IdentifierHash& id : rpcHashList) {

       auto RPCcoll = rpcPrds->indexFindPtr(id);

       if( RPCcoll == nullptr ) {
         continue;
       }

       if( RPCcoll->size() == 0) {
         ATH_MSG_DEBUG("Empty RPC list");
         continue;
       }

       rpcHashList_cache.push_back(id);
       rpcCols.push_back(RPCcoll);
     }
   }
   ATH_MSG_DEBUG("Do rpc clustering");
   ATH_CHECK( m_clusterPreparator->clusteringRPCs(m_doMultiMuon, rpcCols, p_roids, clusterPatFinder, rpcLayerClusters) );

  return StatusCode::SUCCESS;
}

