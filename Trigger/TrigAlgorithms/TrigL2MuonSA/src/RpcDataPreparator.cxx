/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "TrigL2MuonSA/RpcDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "Identifier/IdentifierHash.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "TrigL2MuonSA/RpcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

using namespace Muon;
using namespace MuonGM;
using namespace SG;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_RpcDataPreparator("IID_RpcDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::RpcDataPreparator::interfaceID() { return IID_RpcDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcDataPreparator::RpcDataPreparator(const std::string& type, 
                                                   const std::string& name,
                                                   const IInterface*  parent): 
   AthAlgTool(type,name,parent),
   m_storeGateSvc( "StoreGateSvc", name ),
   m_activeStore(0),
   m_regionSelector(0),
   m_rpcPrepDataProvider("Muon::RpcRdoToPrepDataTool/RpcPrepDataProviderTool"),
   m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
{
   declareInterface<TrigL2MuonSA::RpcDataPreparator>(this);
   declareProperty("RpcPrepDataProvider", m_rpcPrepDataProvider);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcDataPreparator::~RpcDataPreparator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::initialize()
{
   // Get a message stream instance
  ATH_MSG_DEBUG("Initializing RpcDataPreparator - package version " << PACKAGE_VERSION);
   
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not find StoreGateSvc");
      return sc;
   }

   // Locate RegionSelector
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
     ATH_MSG_ERROR("Could not retrieve RegionSelector");
      return sc;
   }
   ATH_MSG_DEBUG("Retrieved service RegionSelector");

   StoreGateSvc* detStore;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if (sc.isFailure()) {
     ATH_MSG_ERROR("Could not retrieve DetectorStore.");
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved DetectorStore.");
 
   sc = detStore->retrieve( m_muonMgr );
   if (sc.isFailure()) return sc;
   ATH_MSG_DEBUG("Retrieved GeoModel from DetectorStore.");
   m_rpcIdHelper = m_muonMgr->rpcIdHelper();

   sc = m_rpcPrepDataProvider.retrieve();
   if (sc.isSuccess()) {
     ATH_MSG_DEBUG("Retrieved " << m_rpcPrepDataProvider);
   } else {
     ATH_MSG_FATAL("Could not get " << m_rpcPrepDataProvider);
     return sc;
   }

   sc = m_idHelperTool.retrieve();
   if (sc.isSuccess()) {
     ATH_MSG_DEBUG("Retrieved " << m_idHelperTool);
   } else {
     ATH_MSG_FATAL("Could not get " << m_idHelperTool); 
     return sc;
   }

   // Retrieve ActiveStore
   sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
   if (sc.isFailure() || m_activeStore == 0) {
     ATH_MSG_ERROR(" Cannot get ActiveStoreSvc.");
     return sc ;
   }
   ATH_MSG_DEBUG("Retrieved ActiveStoreSvc."); 

   // Retrieve the RPC cabling service
   ServiceHandle<IRPCcablingServerSvc> RpcCabGet ("RPCcablingServerSvc", name());
   sc = RpcCabGet.retrieve();
   if ( sc != StatusCode::SUCCESS ) {
     ATH_MSG_ERROR("Could not retrieve the RPCcablingServerSvc");
     return sc;
   }
   sc = RpcCabGet->giveCabling(m_rpcCabling);
   if ( sc != StatusCode::SUCCESS ) {
     ATH_MSG_ERROR("Could not retrieve the RPC Cabling Server");
     return sc;
   }
   m_rpcCablingSvc = m_rpcCabling->getRPCCabling();
   if ( !m_rpcCablingSvc ) {
     ATH_MSG_ERROR("Could not retrieve the RPC cabling svc");
     return StatusCode::FAILURE;
   } 
   
   ATH_CHECK(m_rpcPrepContainerKey.initialize());
   
   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcDataPreparator::setRoIBasedDataAccess(bool use_RoIBasedDataAccess)
{
  m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;
  return;
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
   std::vector<IdentifierHash>  rpcHashList_cache;

   if (m_use_RoIBasedDataAccess) {

     ATH_MSG_DEBUG("Use RoI based data access");
     
     if (iroi) m_regionSelector->DetHashIDList(RPC, *iroi, rpcHashList);
     else m_regionSelector->DetHashIDList(RPC, rpcHashList);
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());
     
     std::vector<uint32_t> rpcRobList;
     m_regionSelector->DetROBIDListUint(RPC, *iroi, rpcRobList);
     if ( m_rpcPrepDataProvider->decode(rpcRobList).isFailure() ) {
       ATH_MSG_WARNING("Problems when preparing RPC PrepData ");
     }
     
   } else {
     
     ATH_MSG_DEBUG("Use full data access");
     
     m_regionSelector->DetHashIDList(RPC, rpcHashList);
     ATH_MSG_DEBUG("rpcHashList.size()=" << rpcHashList.size());
     
     std::vector<uint32_t> rpcRobList;
     m_regionSelector->DetROBIDListUint(RPC, rpcRobList);
     if ( m_rpcPrepDataProvider->decode(rpcRobList).isFailure() ) {
       ATH_MSG_WARNING("Problems when preparing RPC PrepData ");
     }
     
   }
   
   if (!rpcHashList.empty()) {
     
     // Get RPC container
     const RpcPrepDataContainer* rpcPrds;
     if (m_activeStore) {		//m_activeStore->m_storeGateSvc
       auto rpcPrepContainerHandle = SG::makeHandle(m_rpcPrepContainerKey);
       rpcPrds = rpcPrepContainerHandle.cptr();                                 
       if (!rpcPrepContainerHandle.isValid()) {
         ATH_MSG_ERROR("Cannot retrieve RPC PRD Container key: " << m_rpcPrepContainerKey.key());
         return StatusCode::FAILURE;
       } else {
         ATH_MSG_DEBUG(" RPC PRD Container retrieved with key: " << m_rpcPrepContainerKey.key());
       }         
     } else {
       ATH_MSG_ERROR("Null pointer to ActiveStore");
       return StatusCode::FAILURE;;
     }
     // Get RPC collections

     RpcPrepDataContainer::const_iterator RPCcoll;
     for(std::vector<IdentifierHash>::const_iterator idit = rpcHashList.begin(); idit != rpcHashList.end(); ++idit) {

       RPCcoll = rpcPrds->indexFind(*idit);

       if( RPCcoll == rpcPrds->end() ) {
         continue;
       }

       if( (*RPCcoll)->size() == 0)    {
         ATH_MSG_DEBUG("Empty RPC list");
         continue;
       }

       rpcHashList_cache.push_back(*idit);
       
       rpcCols.push_back(*RPCcoll);

       if (rpcCols.empty()) {
         ATH_MSG_DEBUG("No Rpc data collections selected");
       }
     }
   }

   std::vector< const RpcPrepDataCollection*>::const_iterator it = rpcCols.begin();
   std::vector< const RpcPrepDataCollection*>::const_iterator it_end = rpcCols.end();

   for( ;it!=it_end;++it ){
     Muon::RpcPrepDataCollection::const_iterator cit_begin = (*it)->begin();
     Muon::RpcPrepDataCollection::const_iterator cit_end = (*it)->end();
     if (cit_begin == cit_end) return StatusCode::SUCCESS;
     
     Muon::RpcPrepDataCollection::const_iterator cit = cit_begin;   
     
     cit = cit_begin;   
     for( ; cit!=cit_end;++cit ) {
       const Muon::RpcPrepData* prd = *cit;
       Identifier id = prd->identify();

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
       
       float r = sqrt(hitx*hitx+hity*hity);
       float phi = atan(hity/hitx);
       if (hitx<0 && hity>0) phi = phi + CLHEP::pi;
       if (hitx<0 && hity<0) phi = phi - CLHEP::pi;
       float l = sqrt(hitz*hitz+r*r);
       float tan = sqrt( (l-hitz)/(l+hitz) );
       float eta = -log(tan);
       float deta = p_roids->eta() - eta;
       float dphi = acos(cos( p_roids->phi() - phi ) );

       lutDigit.eta = eta;
       lutDigit.phi = phi;
       lutDigit.l = l;
       rpcHits.push_back(lutDigit);
       
       if (m_use_RoIBasedDataAccess) {
         if ( fabs(deta)<0.1 && fabs(dphi)<0.1)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       } else {
         if ( fabs(deta)<0.15 && fabs(dphi)<0.1)
           (*rpcPatFinder)->addHit(stationName, stationEta, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       }
     }
   }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::finalize()
{
  ATH_MSG_DEBUG("Finalizing RpcDataPreparator - package version " << PACKAGE_VERSION);
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

