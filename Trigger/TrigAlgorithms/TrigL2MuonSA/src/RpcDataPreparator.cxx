/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>

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
   AthAlgTool(type,name,parent),
   m_regionSelector( "RegSelSvc", name )
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::initialize()
{

   // Locate RegionSelector
   ATH_CHECK( m_regionSelector.retrieve() );
   ATH_MSG_DEBUG("Retrieved service RegionSelector");

   ATH_CHECK( m_recRPCRoiSvc.retrieve() );
   ATH_MSG_DEBUG( "Retrieved Service " << m_recRPCRoiSvc );

   // consistency check for decoding flag settings
   if(m_decodeBS && !m_doDecoding) {
     ATH_MSG_FATAL("Inconsistent setup, you tried to enable BS decoding but disable all decoding. Please fix the configuration");
     return StatusCode::FAILURE;
   }
 
   // disable the RDO->PRD decoding tool if we don't do the RPC data decoding
   ATH_CHECK( m_rpcPrepDataProvider.retrieve(DisableTool{!m_doDecoding}) );
   ATH_MSG_DEBUG("Retrieved " << m_rpcPrepDataProvider);

   ATH_CHECK(m_idHelperSvc.retrieve());
   ATH_MSG_DEBUG("Retrieved " << m_idHelperSvc);

   // Retreive PRC raw data provider tool
   ATH_MSG_DEBUG("Decode BS set to " << m_decodeBS);
   // disable the BS->RDO decoding tool if we don't do the RPC data decoding
   ATH_CHECK( m_rawDataProviderTool.retrieve(DisableTool{ !m_decodeBS || !m_doDecoding }) );
   ATH_MSG_DEBUG("Retrieved Tool " << m_rawDataProviderTool);

   ATH_CHECK(m_readKey.initialize());
   ATH_CHECK(m_rpcPrepContainerKey.initialize());

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
                                                        unsigned int roiWord,
                                                        TrigL2MuonSA::RpcHits&      rpcHits,
                                                        ToolHandle<RpcPatFinder>*   rpcPatFinder)
{
  // RPC data extraction referring TrigMuonEFStandaloneTrackTool and MuonHoughPatternFinderTool
  rpcHits.clear();
  
  // set to false the flag indicating whether the roi is a fake one.
  m_isFakeRoi = false;

  if( m_emulateNoRpcHit )
    return StatusCode::SUCCESS;

  // check the roi ID
  
  //  decode  roIWord
  unsigned int sectorAddress = (roiWord & 0x003FC000) >> 14;
  unsigned int sectorRoIOvl  = (roiWord & 0x000007FC) >> 2;
  unsigned int side =  sectorAddress & 0x00000001;
  unsigned int sector = (sectorAddress & 0x0000003e) >> 1;
  unsigned int roiNumber =  sectorRoIOvl & 0x0000001F;

  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey};
  const RpcCablingCondData* readCdo{*readHandle};
  unsigned int padIdHash;
  if ( !readCdo->give_PAD_address( side, sector, roiNumber, padIdHash) ) {
    ATH_MSG_WARNING("Roi Number: " << roiNumber << " not compatible with side, sector: "
        << side <<  " " << sector << " (padIdHash=" << padIdHash << ")");
    // set the bool flag to send the event to the debug stream
    m_isFakeRoi = true;
  }
  else {
    ATH_MSG_DEBUG("Roi Number: " << roiNumber << " side, sector: " << side <<  " " << sector);
  }

   const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;

   std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
   std::vector<IdentifierHash> rpcHashList;
   std::vector<IdentifierHash> rpcHashList_cache;

   if (m_use_RoIBasedDataAccess) {

     ATH_MSG_DEBUG("Use RoI based data access");
     
     if (iroi) m_regionSelector->DetHashIDList(RPC, *iroi, rpcHashList);
     else m_regionSelector->DetHashIDList(RPC, rpcHashList);
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());
     
     std::vector<uint32_t> rpcRobList;
     m_regionSelector->DetROBIDListUint(RPC, *iroi, rpcRobList);
     if(m_doDecoding) {
       if(m_decodeBS) {
         if ( m_rawDataProviderTool->convert(rpcRobList).isFailure()) {
             ATH_MSG_WARNING("Conversion of BS for decoding of RPCs failed");
         }
       }
       if ( m_rpcPrepDataProvider->decode(rpcRobList).isFailure() ) {
         ATH_MSG_WARNING("Problems when preparing RPC PrepData ");
       }
     }//do decoding
     
   } else {
     
     ATH_MSG_DEBUG("Use full data access");
     
     m_regionSelector->DetHashIDList(RPC, rpcHashList);
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());
     
     std::vector<uint32_t> rpcRobList;
     m_regionSelector->DetROBIDListUint(RPC, rpcRobList);
     if(m_doDecoding) {
       if(m_decodeBS) {
         if ( m_rawDataProviderTool->convert(rpcRobList).isFailure()) {
             ATH_MSG_WARNING("Conversion of BS for decoding of RPCs failed");
         }
       }
       if ( m_rpcPrepDataProvider->decode(rpcRobList).isFailure() ) {
         ATH_MSG_WARNING("Problems when preparing RPC PrepData ");
       }
     }//do decoding
     
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
       float phi = atan2(hity,hitx);
       const float l = sqrt(hitz*hitz+r2);
       const float tan = sqrt( (l-hitz)/(l+hitz) );
       const float eta = -log(tan);
       const float deta = fabs(p_roids->eta() - eta);
       const float dphi = fabs(CxxUtils::wrapToPi(p_roids->phi() - phi));

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
         m_recRPCRoiSvc->reconstruct( roiWord );
         float RoiPhiMin = m_recRPCRoiSvc->phiMin();
         float RoiPhiMax = m_recRPCRoiSvc->phiMax();
         float RoiEtaMin = m_recRPCRoiSvc->etaMin();
         float RoiEtaMax = m_recRPCRoiSvc->etaMax();
         ATH_MSG_DEBUG( "RoI Phi min = " << RoiPhiMin << " RoI Phi max = " << RoiPhiMax << " RoI Eta min = " << RoiEtaMin << " RoI Eta max = " << RoiEtaMax );
         deta_thr = std::abs( RoiEtaMax - RoiEtaMin )/2. + dynamic_add;
         dphi_thr = std::abs( acos( cos( RoiPhiMax - RoiPhiMin ) ) )/2. + dynamic_add;
         ATH_MSG_DEBUG( "deta threshold = " << deta_thr);
         ATH_MSG_DEBUG( "dphi threshold = " << dphi_thr);
       }

       if (m_use_RoIBasedDataAccess) {
         if ( deta<deta_thr && dphi<dphi_thr)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       } else {
         if ( deta<0.15 && dphi<0.1)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       }
     }
   }

  return StatusCode::SUCCESS;
}
