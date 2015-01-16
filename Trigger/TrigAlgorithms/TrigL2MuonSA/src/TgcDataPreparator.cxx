/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/TgcDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "Identifier/IdentifierHash.h"

#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"

#include "StoreGate/ActiveStoreSvc.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

using namespace MuonGM;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_TgcDataPreparator("IID_TgcDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::TgcDataPreparator::interfaceID() { return IID_TgcDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcDataPreparator::TgcDataPreparator(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
  AthAlgTool(type,name,parent),
   m_msg(0),
   m_storeGateSvc( "StoreGateSvc", name ),
   m_tgcPrepDataProvider("Muon::TgcRdoToPrepDataTool/TgcPrepDataProviderTool"),
   m_tgcRawDataProvider("Muon__TGC_RawDataProviderTool"),
   m_regionSelector(0), m_robDataProvider(0),
   m_options(), m_recMuonRoIUtils()
{
   declareInterface<TrigL2MuonSA::TgcDataPreparator>(this);
   declareProperty("TgcPrepDataProvider", m_tgcPrepDataProvider);
   declareProperty("TGC_RawDataProvider", m_tgcRawDataProvider);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcDataPreparator::~TgcDataPreparator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing TgcDataPreparator - package version " << PACKAGE_VERSION << endreq ;
   
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

   // Locate TGC RawDataProvider
   sc = m_tgcRawDataProvider.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_tgcRawDataProvider << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved tool " << m_tgcRawDataProvider << endreq;

   // locate new cabling service
   sc = service("MuonTGC_CablingSvc",m_tgcCabling);
   if (sc.isFailure() ){
      msg() << MSG::ERROR << "Could not retrieve " << m_tgcCabling << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << m_tgcCabling << endreq;

   // Locate RegionSelector
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve RegionSelector" << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service RegionSelector" << endreq;

   // Locate ROBDataProvider
   std::string serviceName = "ROBDataProvider";
   IService* svc = 0;
   sc = service("ROBDataProviderSvc", svc);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve " << serviceName << endreq;
      return sc;
   }
   m_robDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
   if( m_robDataProvider == 0 ) {
      msg() << MSG::ERROR << "Could not cast to ROBDataProviderSvc " << endreq;
      return StatusCode::FAILURE;
   }
   msg() << MSG::DEBUG << "Retrieved service " << serviceName << endreq;

   StoreGateSvc* detStore(0);
   sc = serviceLocator()->service("DetectorStore", detStore);
   if (sc.isFailure()) {
     msg() << MSG::ERROR << "Could not retrieve DetectorStore." << endreq;
     return sc;
   }
   msg() << MSG::DEBUG << "Retrieved DetectorStore." << endreq;
 
   sc = detStore->retrieve( m_muonMgr,"Muon" );
   if (sc.isFailure()) return sc;
   msg() << MSG::DEBUG << "Retrieved GeoModel from DetectorStore." << endreq;
   m_tgcIdHelper = m_muonMgr->tgcIdHelper();

   sc = m_tgcPrepDataProvider.retrieve();
   if (sc.isFailure()) return sc;
   msg() << MSG::DEBUG << "Retrieved m_tgcPrepDataProvider" << endreq;

   sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
   if (sc.isFailure()) {
     msg() << MSG::ERROR << " Cannot get ActiveStoreSvc." << endreq;
     return sc ;
   }
   msg() << MSG::DEBUG << "Retrieved ActiveStoreSvc." << endreq; 
   
   
   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::prepareData(const LVL1::RecMuonRoI*  p_roi,
							TrigL2MuonSA::TgcHits&  tgcHits)
{
   float roi_eta = p_roi->eta();
   float roi_phi = p_roi->phi();
   if (roi_phi < 0) roi_phi += 2.0 * CLHEP::pi;
   
   double etaMin = p_roi->eta() - 0.2;
   double etaMax = p_roi->eta() + 0.2;
   double phiMin = p_roi->phi() - 0.1;
   double phiMax = p_roi->phi() + 0.1;
   if( phiMin < 0 ) phiMin += 2*CLHEP::pi;
   if( phiMax < 0 ) phiMax += 2*CLHEP::pi;
   if( phiMin > 2*CLHEP::pi ) phiMin -= 2*CLHEP::pi;
   if( phiMax > 2*CLHEP::pi ) phiMax -= 2*CLHEP::pi;

   TrigRoiDescriptor* roi = new TrigRoiDescriptor( p_roi->eta(), etaMin, etaMax, p_roi->phi(), phiMin, phiMax ); 
   const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;
   
   const Muon::TgcPrepDataContainer* tgcPrepContainer = 0;
   int gasGap;
   int channel;
   
   bool isLowPt = m_recMuonRoIUtils.isLowPt(p_roi);
   
   // Select the eta cut based on ROI Pt.
   double mid_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtMiddleForLowPt()
     : m_options.roadParameters().deltaEtaAtMiddleForHighPt();
   double inn_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtInnerForLowPt()
     : m_options.roadParameters().deltaEtaAtInnerForHighPt();
   double mid_phi_test = m_options.roadParameters().deltaPhiAtMiddle();
   double inn_phi_test = m_options.roadParameters().deltaPhiAtInner();
   
   // start conversion from RAW to RDO
   std::vector<uint32_t> v_robIds;
   std::vector<IdentifierHash> tgcHashList;
   IdContext tgcContext = m_tgcIdHelper->module_context();
   int maxRodId = 0;
   int maxSswId = 0;
   int maxSbloc = 0;
   int minChannelId = 0;
   int maxChannelId = 0;
   m_tgcCabling->getReadoutIDRanges(maxRodId, maxSswId, maxSbloc, minChannelId, maxChannelId);
   bool isAtlas = (maxRodId==12); // should be true
   int offset = (isAtlas)? -1 : 0; // should be -1
   
   m_regionSelector->DetHashIDList(TGC, *iroi, tgcHashList);
   if(roi) delete roi;
   bool redundant;
   for(int hash_iter=0; hash_iter<(int)tgcHashList.size(); hash_iter++){
     redundant = false;
     // get ROB id
     Identifier rdoId;
     m_tgcIdHelper->get_id(tgcHashList[hash_iter], rdoId, &tgcContext);
     int subDetectorId = 0; // 0x67 (A side) or 0x68 (C side)
     int rodId = 0; // 1-12
     m_tgcCabling->getReadoutIDfromElementID(rdoId, subDetectorId, rodId);
     int isAside = (subDetectorId==0x67) ? 0 : 1;
     uint32_t newROBId = static_cast<uint32_t>(isAside*(maxRodId+1+offset) + rodId + offset); // 0-23
     // end part to get ROB id
     for (int rob_iter=0; rob_iter<(int)v_robIds.size(); rob_iter++){
       if(newROBId == v_robIds[rob_iter])
	 redundant = true;
     }
     if(!redundant)
       v_robIds.push_back(newROBId);
   }
   
   m_robDataProvider->addROBData(v_robIds);
   std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> v_robFragments;
   
   const TgcRdoContainer* pRdoContainer = Muon::MuonRdoContainerAccess::retrieveTgcRdo("TGCRDO");
   if( pRdoContainer==0 ) {
     msg() << MSG::DEBUG << "Tgc RDO container not registered by MuonRdoContainerManager" << endreq;
     msg() << MSG::DEBUG << "-> Retrieving it from the StoreGate" << endreq;
     StatusCode sc = m_storeGateSvc->retrieve(pRdoContainer, "TGCRDO");
     if( sc.isFailure() ) {
       msg() << MSG::ERROR << "Retrieval of TgcRdoContainer failed" << endreq;
       return sc;
     }
   }
   
   m_robDataProvider->getROBData(v_robIds,v_robFragments);
   
   if( m_tgcRawDataProvider->convert(v_robFragments).isFailure() ) {
     msg() << MSG::ERROR << "Failed to convert v_robIds" << endreq;
     return StatusCode::FAILURE;
   }
   
   // now convert from RDO to PRD
   std::vector<IdentifierHash> inhash, outhash;
   inhash = tgcHashList; 
   
   if( m_tgcPrepDataProvider->decode(inhash, outhash).isFailure() ){
     msg() << MSG::ERROR << "Failed to convert from RDO to PRD" << endreq;
     return StatusCode::FAILURE;
   }
   
   StatusCode sc_read = (*m_activeStore)->retrieve( tgcPrepContainer, "TGC_Measurements" );
   if (sc_read.isFailure()){
     msg() << MSG::ERROR << "Could not retrieve PrepDataContainer." << endreq;
     return sc_read;
   }
   msg() << MSG::DEBUG << "Retrieved PrepDataContainer: " << tgcPrepContainer->numberOfCollections() << endreq;
   
   Muon::TgcPrepDataContainer::const_iterator it = tgcPrepContainer->begin();
   Muon::TgcPrepDataContainer::const_iterator it_end = tgcPrepContainer->end();
   for( ; it!=it_end; ++it ) { // loop over collections
     const Muon::TgcPrepDataCollection* col = *it;
     if( !col ) continue;
     Muon::TgcPrepDataCollection::const_iterator cit = col->begin();
     Muon::TgcPrepDataCollection::const_iterator cit_end = col->end();
     for( ;cit!=cit_end;++cit ){ // loop over data in the collection
       if( !*cit ) continue;
       
       const Muon::TgcPrepData& prepData = **cit;
       
       bool isInRoad = false;
       int stationNum = m_tgcIdHelper->stationRegion(prepData.identify())-1;
       if (stationNum==-1) stationNum=3;
       if (m_tgcIdHelper->isStrip(prepData.identify())) {
	 double dphi = fabs(prepData.globalPosition().phi() - roi_phi);
	 if( dphi > CLHEP::pi*2 ) dphi = dphi - CLHEP::pi*2;
	 if( dphi > CLHEP::pi ) dphi = CLHEP::pi*2 - dphi;
	 // For strips, apply phi cut
	 if     ( stationNum < 3  && dphi < mid_phi_test ) { isInRoad = true; }
	 else if( stationNum == 3 && dphi < inn_phi_test ) { isInRoad = true; }
       }
       else {
	 // For wires, apply eta cut.
	 if     ( stationNum < 3  && fabs(prepData.globalPosition().eta() - roi_eta) < mid_eta_test ) { isInRoad = true; }
	 else if( stationNum == 3 && fabs(prepData.globalPosition().eta() - roi_eta) < inn_eta_test ) { isInRoad = true; }
       }
       if( ! isInRoad ) continue;
       
       m_tgcReadout = prepData.detectorElement();
       gasGap = m_tgcIdHelper->gasGap(prepData.identify());
       channel = m_tgcIdHelper->channel(prepData.identify());
       
       TrigL2MuonSA::TgcHitData lutDigit;
       
       lutDigit.eta = prepData.globalPosition().eta();
       lutDigit.phi = prepData.globalPosition().phi();
       lutDigit.r = prepData.globalPosition().perp();
       lutDigit.z = prepData.globalPosition().z();
       lutDigit.sta = stationNum;
       lutDigit.isStrip = m_tgcIdHelper->isStrip(prepData.identify());
       if(m_tgcIdHelper->isStrip(prepData.identify())){
	 lutDigit.width = m_tgcReadout->stripWidth(gasGap, channel);
       }
       else{
	 lutDigit.width = m_tgcReadout->gangLength(gasGap, channel);
       }
       lutDigit.bcTag = 2;
       lutDigit.inRoad = false;
       
       tgcHits.push_back(lutDigit);
       
     }
   }
   
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::finalize()
{
   msg() << MSG::DEBUG << "Finalizing TgcDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
