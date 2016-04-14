/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "TrigL2MuonSA/RpcDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
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

using namespace Muon;
using namespace MuonGM;

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
   m_msg(0),
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
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing RpcDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AthAlgTool base class." << endreq;
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return sc;
   }

   // Locate RegionSelector
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve RegionSelector" << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service RegionSelector" << endreq;

   StoreGateSvc* detStore;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if (sc.isFailure()) {
     msg() << MSG::ERROR << "Could not retrieve DetectorStore." << endreq;
     return sc;
   }
   msg() << MSG::DEBUG << "Retrieved DetectorStore." << endreq;
 
   sc = detStore->retrieve( m_muonMgr );
   if (sc.isFailure()) return sc;
   msg() << MSG::DEBUG << "Retrieved GeoModel from DetectorStore." << endreq;
   m_rpcIdHelper = m_muonMgr->rpcIdHelper();

   sc = m_rpcPrepDataProvider.retrieve();
   if (sc.isSuccess()) {
     msg() << MSG::DEBUG << "Retrieved " << m_rpcPrepDataProvider << endreq;
   } else {
     msg() << MSG::FATAL << "Could not get " << m_rpcPrepDataProvider << endreq;
     return sc;
   }

   sc = m_idHelperTool.retrieve();
   if (sc.isSuccess()) {
     msg() << MSG::DEBUG << "Retrieved " << m_idHelperTool << endreq;
   } else {
     msg() << MSG::FATAL<<"Could not get " << m_idHelperTool <<endreq; 
     return sc;
   }

   // Retrieve ActiveStore
   sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
   if (sc.isFailure() || m_activeStore == 0) {
     msg() << MSG::ERROR << " Cannot get ActiveStoreSvc." << endreq;
     return sc ;
   }
   msg() << MSG::DEBUG << "Retrieved ActiveStoreSvc." << endreq; 

   // Retrieve the RPC cabling service
   ServiceHandle<IRPCcablingServerSvc> RpcCabGet ("RPCcablingServerSvc", name());
   sc = RpcCabGet.retrieve();
   if ( sc != StatusCode::SUCCESS ) {
     msg() << MSG::ERROR << "Could not retrieve the RPCcablingServerSvc" << endreq;
     return sc;
   }
   sc = RpcCabGet->giveCabling(m_rpcCabling);
   if ( sc != StatusCode::SUCCESS ) {
     msg() << MSG::ERROR << "Could not retrieve the RPC Cabling Server" << endreq;
     return sc;
   }
   m_rpcCablingSvc = m_rpcCabling->getRPCCabling();
   if ( !m_rpcCablingSvc ) {
     msg() << MSG::ERROR << "Could not retrieve the RPC cabling svc" << endreq;
     return StatusCode::FAILURE;
   } 
   
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
                                                        TrigL2MuonSA::RpcPatFinder* rpcPatFinder)
{
  // RPC data extraction referring TrigMuonEFStandaloneTrackTool and MuonHoughPatternFinderTool
  rpcHits.clear();
  
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
    msg() << MSG::WARNING << "Roi Number: " << roiNumber << " not compatible with side, sector: " << side 
	  <<  " " << sector << endreq;
    //    return StatusCode::FAILURE;
  }
  else {
    msg() << MSG::DEBUG << "Roi Number: " << roiNumber << " side, sector: " << side 
	  <<  " " << sector << " corresp. to log_sector, padId: " << logic_sector << " " << PADId << endreq;
  }

   const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;

   std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
   std::vector<IdentifierHash> rpcHashList;
   std::vector<IdentifierHash>  rpcHashList_cache;

   if (m_use_RoIBasedDataAccess) {

     msg() << MSG::DEBUG << "Use RoI based data access" << endreq;
     
     if (iroi) m_regionSelector->DetHashIDList(RPC, *iroi, rpcHashList);
     else m_regionSelector->DetHashIDList(RPC, rpcHashList);
     msg() << MSG::DEBUG << "rpcHashList.size()=" << rpcHashList.size() << endreq;
     
     std::vector<uint32_t> rpcRobList;
     m_regionSelector->DetROBIDListUint(RPC, *iroi, rpcRobList);
     if ( m_rpcPrepDataProvider->decode(rpcRobList).isFailure() ) {
       msg() << MSG::WARNING << "Problems when preparing RPC PrepData " << endreq;
     }
     
   } else {
     
     msg() << MSG::DEBUG << "Use full data access" << endreq;
     
     m_regionSelector->DetHashIDList(RPC, rpcHashList);
     msg() << MSG::DEBUG << "rpcHashList.size()=" << rpcHashList.size() << endreq;
     
     std::vector<uint32_t> rpcRobList;
     m_regionSelector->DetROBIDListUint(RPC, rpcRobList);
     if ( m_rpcPrepDataProvider->decode(rpcRobList).isFailure() ) {
       msg() << MSG::WARNING << "Problems when preparing RPC PrepData " << endreq;
     }
     
   }
   
   if (!rpcHashList.empty()) {
     
     // Get RPC container
     const RpcPrepDataContainer* rpcPrds = 0;
     std::string rpcKey = "RPC_Measurements";
     
     if (m_activeStore) {
       StatusCode sc = (*m_activeStore)->retrieve(rpcPrds, rpcKey);
       if ( sc.isFailure() ) {
         msg() << MSG::ERROR << " Cannot retrieve RPC PRD Container " << rpcKey << endreq;
         return StatusCode::FAILURE;;
       } else {       
         msg()<< MSG::DEBUG << " RPC PRD Container retrieved with key " << rpcKey << endreq;
       }
     } else {
       msg() << MSG::ERROR << "Null pointer to ActiveStore" << endreq;
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
         msg() << MSG::DEBUG << "Empty RPC list" << endreq;
         continue;
       }

       rpcHashList_cache.push_back(*idit);
       
       rpcCols.push_back(*RPCcoll);

       if (rpcCols.empty()) {
         msg() << MSG::DEBUG << "No Rpc data collections selected" << endreq;
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

       const int doubletR = m_rpcIdHelper->doubletR(id);
       const int doubletPhi = m_rpcIdHelper->doubletPhi(id);
       const int doubletZ = m_rpcIdHelper->doubletZ(id);
       const int gasGap = m_rpcIdHelper->gasGap(id);
       const bool measuresPhi = m_rpcIdHelper->measuresPhi(id);
       std::string stationName = m_rpcIdHelper->stationNameString(m_rpcIdHelper->stationName(id));
       
       int layer = 0;
       // BO 
       if (stationName.substr(0,2)=="BO") layer = 4;
       // doubletR
       layer += 2*(doubletR-1);
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

       msg() << MSG::DEBUG << "Selected Rpc Collection: "
             << " station name:" << stationName
             << " global positions x/y/z=" << hitx << "/" << hity << "/" << hitz
             << " doubletR: " << doubletR << " doubletZ: " << doubletZ << " doubletPhi " << doubletPhi
             << " gasGap " << gasGap << " layer " << layer << " time " << hittime
	     << " distToEtaReadout " << distToEtaReadout << " distToPhiReadout " << distToPhiReadout
             << endreq;
       
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
           rpcPatFinder->addHit(stationName, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       } else {
         if ( fabs(deta)<0.15 && fabs(dphi)<0.1) 
           rpcPatFinder->addHit(stationName, measuresPhi, gasGap, doubletR, hitx, hity, hitz);
       }
     }
   }
     
     return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcDataPreparator::finalize()
{
   msg() << MSG::DEBUG << "Finalizing RpcDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

