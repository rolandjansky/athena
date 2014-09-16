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

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_TgcDataPreparator("IID_TgcDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::TgcDataPreparator::interfaceID() { return IID_TgcDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcDataPreparator::TgcDataPreparator(const std::string& type, 
						 const std::string& name,
						 const IInterface*  parent): 
   AlgTool(type,name,parent),
   m_msg(0),
   m_storeGateSvc( "StoreGateSvc", name ),
   m_tgcRawDataProvider("Muon__TGC_RawDataProviderTool"),
   m_tgcGeometrySvc("TGCgeometrySvc",""),
   m_robDataProvider(0),
   m_options(), m_recMuonRoIUtils()
{
   declareInterface<TrigL2MuonSA::TgcDataPreparator>(this);

   declareProperty("TGC_RawDataProvider", m_tgcRawDataProvider);
   declareProperty("TGC_GeometrySvc",     m_tgcGeometrySvc);
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
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
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

   // Locate TGC GeometrySvc
   sc = m_tgcGeometrySvc.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_tgcGeometrySvc << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << m_tgcGeometrySvc << endreq;

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

   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::prepareData(const LVL1::RecMuonRoI* p_roi,
							TrigL2MuonSA::TgcHits& tgcHits)
{
   float roi_eta = p_roi->eta();
   float roi_phi = p_roi->phi();
   if (roi_phi < 0) roi_phi += 2.0 * CLHEP::pi;
   unsigned int robId = m_tgcGeometrySvc->robNumber(roi_eta,roi_phi);

   m_tgcRawData.clear();

   const TgcRdo* p_tgcRdo = 0;
   StatusCode sc = getTgcRdo(robId,roi_eta,roi_phi,p_tgcRdo);
   if( sc != StatusCode::SUCCESS ) {
      msg() << MSG::DEBUG << "getTgcRdo failed: sc=" << sc << endreq;
      return sc;
   }

   sc = decodeTgcRdo(p_tgcRdo,roi_eta,roi_phi,m_recMuonRoIUtils.isLowPt(p_roi),tgcHits);
   if( sc != StatusCode::SUCCESS ) {
      msg() << MSG::DEBUG << "decodeTgcRdo failed: sc=" << sc << endreq;
      return sc;
   }

   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::decodeTgcRdo(const TgcRdo* p_rdo,
							 double roiEta,
							 double roiPhi,
							 bool isLowPt,
							 TrigL2MuonSA::TgcHits& tgcHits)
{
   tgcHits.clear();

   // Select the eta cut based on ROI Pt.
   double mid_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtMiddleForLowPt()
                                   : m_options.roadParameters().deltaEtaAtMiddleForHighPt();
   double inn_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtInnerForLowPt()
                                   : m_options.roadParameters().deltaEtaAtInnerForHighPt();
   double mid_phi_test = m_options.roadParameters().deltaPhiAtMiddle();
   double inn_phi_test = m_options.roadParameters().deltaPhiAtInner();

   // Loop over RawData, converter RawData to digit
   // retrieve/create digit collection, and insert digit into collection
   for (TgcRdo::const_iterator itD = p_rdo->begin(); itD != p_rdo->end(); ++itD) {
     TgcRawData* pRawData = *itD;
     
     // check Hit or Coincidence
     if (pRawData->isCoincidence()) continue;
     
     // select current Hits
     //     if( m_options.isOnTimeOnly() && 
     //	 pRawData->bcTag() != 0 && pRawData->bcTag() != 2 ) continue;

     tgcRawData rawData;
     rawData.bcTag  = pRawData->bcTag();
     rawData.sswId  = pRawData->sswId();
     rawData.slbId  = pRawData->slbId();
     rawData.bitpos = pRawData->bitpos();

     for (std::vector<tgcRawData>::const_iterator itR = m_tgcRawData.begin(); itR != m_tgcRawData.end(); ++itR) {
       if(rawData.sswId == itR->sswId && rawData.slbId == itR->slbId && rawData.bitpos == itR->bitpos) {
         rawData.bitpos = 999;
         break;
       }
     }
     
     if (rawData.bitpos == 999) continue;
     m_tgcRawData.push_back(rawData);

     const TGCgeometrySvc::Entry& e =
	 m_tgcGeometrySvc->getEntry(pRawData->subDetectorId(),
				    pRawData->rodId(),
				    pRawData->sswId(),
				    pRawData->slbId(),
				    pRawData->bitpos());

      // select only within road
      bool isInRoad = false;
      if (e.isStrip) {
	 double dphi = fabs(e.phi - roiPhi);
	 if( dphi > CLHEP::pi*2 ) dphi = dphi - CLHEP::pi*2;
	 if( dphi > CLHEP::pi ) dphi = CLHEP::pi*2 - dphi;
	 // For strips, apply phi cut
	 if     ( e.sta < 3  && dphi < mid_phi_test ) { isInRoad = true; }
	 else if( e.sta == 3 && dphi < inn_phi_test ) { isInRoad = true; }
      }
      else {
	 // For wires, apply eta cut.
	 if     ( e.sta < 3  && fabs(e.eta - roiEta) < mid_eta_test ) { isInRoad = true; }
	 else if( e.sta == 3 && fabs(e.eta - roiEta) < inn_eta_test ) { isInRoad = true; }
      }
      if( ! isInRoad ) continue;

      TrigL2MuonSA::TgcHitData lutDigit;
      lutDigit.eta     = e.eta;
      lutDigit.phi     = e.phi;
      lutDigit.r       = e.r;
      lutDigit.z       = e.z;
      lutDigit.width   = e.width;
      lutDigit.sta     = e.sta;
      lutDigit.isStrip = e.isStrip;
      lutDigit.bcTag   = pRawData->bcTag();
      lutDigit.inRoad  = false;

      tgcHits.push_back(lutDigit);

      if( msgLvl() <= MSG::DEBUG ) {
	 msg() << MSG::DEBUG << "lutDigit r=" << lutDigit.r << " z=" << lutDigit.z << " phi=" << lutDigit.phi
	       <<" station=" << lutDigit.sta << " strip=" << lutDigit.isStrip << " bc=" << lutDigit.bcTag << endreq;
      }
   }

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::getTgcRdo(unsigned int robId,
						      float eta,
						      float phi,
						      const TgcRdo*& p_rdo)
{
   std::vector<uint32_t> v_robIds;
   v_robIds.push_back(robId);
   
   m_robDataProvider->addROBData(v_robIds);

   p_rdo = 0;

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
      msg() << MSG::ERROR << "Failed to convert TGC ROB ID=" << robId << endreq;
      return StatusCode::FAILURE;
   }

   unsigned int rdoId = m_tgcGeometrySvc->rdoID(eta, phi); // identifier for accessing the TGC data
   IdentifierHash idHash = pRdoContainer->idToHash(rdoId);
   TgcRdoContainer::const_iterator itTgc = pRdoContainer->indexFind(idHash);
   if( itTgc==pRdoContainer->end() )  {
      msg() << MSG::DEBUG << "Failed to retrieve TGC hash Id=" << idHash << endreq;
      return StatusCode::FAILURE;
   }

   p_rdo = *itTgc;

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::finalize()
{
   msg() << MSG::DEBUG << "Finalizing TgcDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
