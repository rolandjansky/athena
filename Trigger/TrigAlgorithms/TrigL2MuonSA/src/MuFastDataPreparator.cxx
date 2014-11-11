/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "TrigT1RPClogic/RPCrawData.h"
#include "TrigT1RPClogic/CMAdata.h"

#include "TrigMuonEvent/MuonFeatureDetails.h"

#include "Identifier/IdentifierHash.h"
#include "RPCcablingInterface/RpcPadIdHash.h" 
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "TrigL2MuonSA/MdtRegionDefiner.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastDataPreparator("IID_MuFastDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastDataPreparator::interfaceID() { return IID_MuFastDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastDataPreparator::MuFastDataPreparator(const std::string& type, 
							 const std::string& name,
							 const IInterface*  parent): 
  AlgTool(type,name,parent),
  m_msg(0),
  m_options(),
  m_rpcDataPreparator("TrigL2MuonSA::RpcDataPreparator"),
  m_tgcDataPreparator("TrigL2MuonSA::TgcDataPreparator"),
  m_mdtDataPreparator("TrigL2MuonSA::MdtDataPreparator"),
  m_cscDataPreparator("TrigL2MuonSA::CscDataPreparator"),
  m_rpcRoadDefiner(0),
  m_tgcRoadDefiner(0),
  m_mdtCablingSvc("MDTcablingSvc","")
{
   declareInterface<TrigL2MuonSA::MuFastDataPreparator>(this);
   declareProperty("MDT_CablingSvc",   m_mdtCablingSvc);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastDataPreparator::~MuFastDataPreparator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastDataPreparator::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing MuFastDataPreparator - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
      return sc;
   }

   // retrieve the ID helper and the region selector
   StoreGateSvc* detStore(0);
   const MuonGM::MuonDetectorManager* muonMgr;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if (sc.isFailure()) {
     msg() << MSG::ERROR << "Could not retrieve DetectorStore." << endreq;
     return sc;
   }
   msg() << MSG::DEBUG << "Retrieved DetectorStore." << endreq;
   
   sc = detStore->retrieve( muonMgr,"Muon" );
   if (sc.isFailure()) return sc;
   msg() << MSG::DEBUG << "Retrieved GeoModel from DetectorStore." << endreq;
   m_mdtIdHelper = muonMgr->mdtIdHelper();
   
   // Locate RegionSelector
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve the regionselector service" << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved the RegionSelector service " << endreq;
   
   if (m_use_rpc) {
     sc = m_rpcDataPreparator.retrieve();
     if ( sc.isFailure() ) {
       msg() << MSG::ERROR << "Could not retrieve " << m_rpcDataPreparator << endreq;
       return sc;
     }
     msg() << MSG::DEBUG << "Retrieved service " << m_rpcDataPreparator << endreq;
   }

   sc = m_tgcDataPreparator.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_tgcDataPreparator << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << m_tgcDataPreparator << endreq;

   sc =m_mdtDataPreparator.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_mdtDataPreparator << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << m_mdtDataPreparator << endreq;
   
   sc =m_cscDataPreparator.retrieve();
   if ( sc.isFailure() ) {
     msg() << MSG::ERROR << "Could not retrieve " << m_cscDataPreparator << endreq;
     return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << m_cscDataPreparator << endreq;

   // Set MsgStream for utils
   m_rpcRoadDefiner = new TrigL2MuonSA::RpcRoadDefiner(m_msg);
   m_tgcRoadDefiner = new TrigL2MuonSA::TgcRoadDefiner(m_msg);

   // set the geometry tools
   m_rpcRoadDefiner->setMdtGeometry(m_regionSelector,m_mdtIdHelper);
   m_tgcRoadDefiner->setMdtGeometry(m_regionSelector,m_mdtIdHelper);


   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastDataPreparator::setOptions(const TrigL2MuonSA::MuFastDataPreparatorOptions& options) 
{
   m_options = options;
   m_tgcDataPreparator->setOptions(options.tgcOptions());
   return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastDataPreparator::setMCFlag(BooleanProperty use_mcLUT)
{
  m_use_mcLUT = use_mcLUT;

  StatusCode sc = StatusCode::SUCCESS;

  if (m_use_mcLUT) {
    sc = serviceLocator()->service("PtEndcapLUTSvc_MC", m_ptEndcapLUTSvc);
  } else {
    sc = serviceLocator()->service("PtEndcapLUTSvc",    m_ptEndcapLUTSvc);
  }
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Could not find PtEndcapLUTSvc" << endreq;
    return sc;
  }

  m_tgcRoadDefiner->setPtLUT(m_ptEndcapLUTSvc);

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastDataPreparator::setRoadWidthForFailure(double rWidth_RPC_Failed,
                                                                double rWidth_TGC_Failed)
{
  m_rpcRoadDefiner->setRoadWidthForFailure(rWidth_RPC_Failed);
  m_tgcRoadDefiner->setRoadWidthForFailure(rWidth_TGC_Failed);
  return;
}

void TrigL2MuonSA::MuFastDataPreparator::setRpcGeometry(bool use_rpc)
{
  m_rpcRoadDefiner->setRpcGeometry(use_rpc);
  m_mdtDataPreparator->setRpcGeometry(use_rpc);

  m_use_rpc = use_rpc;
  return;
}

StatusCode TrigL2MuonSA::MuFastDataPreparator::setGeometry(bool use_new_geometry)
{
   m_use_new_geometry = use_new_geometry;

   // Locate MDT CablingSvc - ONLY FOR OLD GEOMETRY / CABLING 
   if (!m_use_new_geometry) {
     StatusCode sc = m_mdtCablingSvc.retrieve();
     if ( sc.isFailure() ) {
       msg() << MSG::ERROR << "Could not retrieve " << m_mdtCablingSvc << endreq;
       return sc;
     }
     msg() << MSG::DEBUG << "Retrieved service " << m_mdtCablingSvc << endreq;
   }

  m_tgcDataPreparator->setGeometry(use_new_geometry);
  m_mdtDataPreparator->setGeometry(use_new_geometry);

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastDataPreparator::setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator)
{
  m_backExtrapolatorTool = backExtrapolator;
  m_tgcRoadDefiner->setExtrapolatorTool(m_backExtrapolatorTool);
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastDataPreparator::prepareData(const LVL1::RecMuonRoI*     p_roi,
							   TrigL2MuonSA::RpcHits&      rpcHits,
							   TrigL2MuonSA::MuonRoad&     muonRoad,
							   TrigL2MuonSA::MdtRegion&    mdtRegion,
							   TrigL2MuonSA::RpcFitResult& rpcFitResult,
							   TrigL2MuonSA::MdtHits&      mdtHits_normal,
							   TrigL2MuonSA::MdtHits&      mdtHits_overlap)
{

  msg() << MSG::DEBUG << "RoI eta/phi=" << p_roi->eta() << "/" << p_roi->phi() << endreq;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if(!m_use_rpc){
  }
  else{
    sc = m_rpcDataPreparator->prepareData(p_roi,
					  rpcHits,
					  rpcFitResult);
    if (!sc.isSuccess()) {
      msg() << MSG::DEBUG << "Error in RPC data prepapration. Continue using RoI" << endreq;
    }
  }
  
  sc = m_rpcRoadDefiner->defineRoad(p_roi,
				    muonRoad,
				    rpcFitResult);
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Error in road definition." << endreq;
    return sc;
  }
  
  sc = m_mdtDataPreparator->prepareData(p_roi,
					rpcFitResult,
					muonRoad,
					mdtRegion,
					mdtHits_normal,
					mdtHits_overlap);
  
  
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Error in MDT data preparation." << endreq;
    return sc;
  }
  msg() << MSG::DEBUG << "nr of MDT (normal)  hits=" << mdtHits_normal.size() << endreq;
  msg() << MSG::DEBUG << "nr of MDT (overlap) hits=" << mdtHits_overlap.size() << endreq;
  
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastDataPreparator::prepareData(const LVL1::RecMuonRoI*     p_roi,
							   TrigL2MuonSA::TgcHits&      tgcHits,
							   TrigL2MuonSA::MuonRoad&     muonRoad,
							   TrigL2MuonSA::MdtRegion&    mdtRegion,
							   TrigL2MuonSA::TgcFitResult& tgcFitResult,
							   TrigL2MuonSA::MdtHits&      mdtHits_normal,
							   TrigL2MuonSA::MdtHits&      mdtHits_overlap,
							   TrigL2MuonSA::CscHits&      cscHits)
{
  StatusCode sc = StatusCode::SUCCESS;
  msg() << MSG::DEBUG << "RoI eta/phi=" << p_roi->eta() << "/" << p_roi->phi() << endreq;
  
  sc = m_tgcDataPreparator->prepareData(p_roi,
					tgcHits);
  if (!sc.isSuccess()) {
    msg() << MSG::DEBUG << "Error in TGC data preparation. Continue using RoI" << endreq;
  }
  msg() << MSG::DEBUG << "nr of TGC hits=" << tgcHits.size() << endreq;
  
  sc = m_tgcRoadDefiner->defineRoad(p_roi,
				    tgcHits,
				    muonRoad,
				    tgcFitResult);
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Error in road definition." << endreq;
    return sc;
  }
  
  sc = m_mdtDataPreparator->prepareData(p_roi,
					tgcFitResult,
					muonRoad,
					mdtRegion,
					mdtHits_normal,
					mdtHits_overlap);

  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Error in MDT data preparation." << endreq;
    return sc;
  }
  msg() << MSG::DEBUG << "nr of MDT (normal)  hits=" << mdtHits_normal.size() << endreq;
  msg() << MSG::DEBUG << "nr of MDT (overlap) hits=" << mdtHits_overlap.size() << endreq;
  
  sc = m_cscDataPreparator->prepareData(p_roi,
					//muonRoad,
					muonRoad.aw[0][0], muonRoad.bw[0][0], muonRoad.rWidth[0][0],
					cscHits);
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Error in CSC data preparation." << endreq;
    return sc;
  }
  msg() << MSG::DEBUG << "nr of CSC hits=" << cscHits.size() << endreq;

  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastDataPreparator::finalize()
{
   msg() << MSG::DEBUG << "Finalizing MuFastDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;

   if ( m_rpcRoadDefiner ) delete m_rpcRoadDefiner;
   if ( m_tgcRoadDefiner ) delete m_tgcRoadDefiner;

   StatusCode sc = AlgTool::finalize(); 
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not finalize the AlgTool base class." << endreq;
      return sc;
   }
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
