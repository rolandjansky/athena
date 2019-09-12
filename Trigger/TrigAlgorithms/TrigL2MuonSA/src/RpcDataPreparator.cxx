/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>

#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"

#include "Identifier/IdentifierHash.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
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

   ATH_CHECK( detStore()->retrieve( m_muonMgr ) );
   ATH_MSG_DEBUG("Retrieved GeoModel from DetectorStore.");
   m_rpcIdHelper = m_muonMgr->rpcIdHelper();
 
   // consistency check for decoding flag settings
   if(m_decodeBS && !m_doDecoding) {
     ATH_MSG_FATAL("Inconsistent setup, you tried to enable BS decoding but disable all decoding. Please fix the configuration");
     return StatusCode::FAILURE;
   }
 
   // disable the RDO->PRD decoding tool if we don't do the RPC data decoding
   ATH_CHECK( m_rpcPrepDataProvider.retrieve(DisableTool{!m_doDecoding}) );
   ATH_MSG_DEBUG("Retrieved " << m_rpcPrepDataProvider);

   ATH_CHECK( m_idHelperTool.retrieve() );
   ATH_MSG_DEBUG("Retrieved " << m_idHelperTool);

   // Retreive PRC raw data provider tool
   ATH_MSG_DEBUG("Decode BS set to " << m_decodeBS);
   // disable the BS->RDO decoding tool if we don't do the RPC data decoding
   ATH_CHECK( m_rawDataProviderTool.retrieve(DisableTool{ !m_decodeBS || !m_doDecoding }) );
   ATH_MSG_DEBUG("Retrieved Tool " << m_rawDataProviderTool);

   // Retrieve the RPC cabling service
   ServiceHandle<IRPCcablingServerSvc> RpcCabGet ("RPCcablingServerSvc", name());
   ATH_CHECK( RpcCabGet.retrieve() ); 
   ATH_CHECK( RpcCabGet->giveCabling(m_rpcCabling) );

   m_rpcCablingSvc = m_rpcCabling->getRPCCabling();
   if ( !m_rpcCablingSvc ) {
     ATH_MSG_ERROR("Could not retrieve the RPC cabling svc");
     return StatusCode::FAILURE;
   } 
   
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
  //  unsigned int padNumber = roiNumber/4; 
  
  unsigned int logic_sector;
  unsigned short int PADId;
  unsigned int padIdHash;
  if ( !m_rpcCablingSvc->give_PAD_address( side, sector, roiNumber, logic_sector, PADId, padIdHash) ) {
    ATH_MSG_WARNING("Roi Number: " << roiNumber << " not compatible with side, sector: "
		    << side <<  " " << sector);
    // set the bool flag to send the event to the debug stream
    m_isFakeRoi = true;
    //    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Roi Number: " << roiNumber << " side, sector: " << side <<  " " << sector
		  << " corresp. to log_sector, padId: " << logic_sector << " " << PADId);
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

       Muon::RpcPrepDataContainer::const_iterator RPCcoll = rpcPrds->indexFind(id);

       if( RPCcoll == rpcPrds->end() ) {
         continue;
       }

       if( (*RPCcoll)->size() == 0) {
         ATH_MSG_DEBUG("Empty RPC list");
         continue;
       }

       rpcHashList_cache.push_back(id);
       rpcCols.push_back(*RPCcoll);
     }
   }

   for( const Muon::RpcPrepDataCollection* rpc : rpcCols ){

     rpcHits.reserve( rpcHits.size() + rpc->size() );
     for( const Muon::RpcPrepData* prd : *rpc ) {

       const Identifier id = prd->identify();

       const int doubletR      = m_rpcIdHelper->doubletR(id);
       const int doubletPhi    = m_rpcIdHelper->doubletPhi(id);
       const int doubletZ      = m_rpcIdHelper->doubletZ(id);
       const int gasGap        = m_rpcIdHelper->gasGap(id);
       const bool measuresPhi  = m_rpcIdHelper->measuresPhi(id);
       const int stationEta    = m_rpcIdHelper->stationEta(id);
       std::string stationName = m_rpcIdHelper->stationNameString(m_rpcIdHelper->stationName(id));

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
       float phi = atan(hity/hitx);
       if (hitx<0 && hity>0) phi += M_PI;
       if (hitx<0 && hity<0) phi -= M_PI;
       const float l = sqrt(hitz*hitz+r2);
       const float tan = sqrt( (l-hitz)/(l+hitz) );
       const float eta = -log(tan);
       const float deta = fabs(p_roids->eta() - eta);
       const float dphi = fabs(CxxUtils::wrapToPi(p_roids->phi() - phi));

       lutDigit.eta = eta;
       lutDigit.phi = phi;
       lutDigit.l = l;
       rpcHits.push_back(lutDigit);
       
       if (m_use_RoIBasedDataAccess) {
         if ( deta<0.1 && dphi<0.1)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       } else {
         if ( deta<0.15 && dphi<0.1)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       }
     }
   }

  return StatusCode::SUCCESS;
}
