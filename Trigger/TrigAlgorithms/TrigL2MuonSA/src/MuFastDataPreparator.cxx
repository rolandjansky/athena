/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "Identifier/IdentifierHash.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastDataPreparator("IID_MuFastDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastDataPreparator::interfaceID() { return IID_MuFastDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastDataPreparator::MuFastDataPreparator(const std::string& type, 
                                                         const std::string& name,
                                                         const IInterface*  parent): 
  AthAlgTool(type,name,parent),
  m_recRPCRoiSvc("LVL1RPC::RPCRecRoiSvc",""),
  m_options(),
  m_rpcDataPreparator("TrigL2MuonSA::RpcDataPreparator"),
  m_tgcDataPreparator("TrigL2MuonSA::TgcDataPreparator"),
  m_mdtDataPreparator("TrigL2MuonSA::MdtDataPreparator"),
  m_cscDataPreparator("TrigL2MuonSA::CscDataPreparator"),
  m_rpcRoadDefiner("TrigL2MuonSA::RpcRoadDefiner"),
  m_tgcRoadDefiner("TrigL2MuonSA::TgcRoadDefiner"),
  m_rpcPatFinder("TrigL2MuonSA::RpcPatFinder")
{
   declareInterface<TrigL2MuonSA::MuFastDataPreparator>(this);
   declareProperty("RPCRecRoiSvc",      m_recRPCRoiSvc,      "Reconstruction of RPC RoI");
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
  ATH_MSG_DEBUG("Initializing MuFastDataPreparator - package version " << PACKAGE_VERSION);
   
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
      return sc;
   }

  sc = m_recRPCRoiSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Couldn't connect to " << m_recRPCRoiSvc);
    return sc;
  } else {
    ATH_MSG_INFO("Retrieved Service " << m_recRPCRoiSvc);
  }
  
  // retrieve the ID helper and the region selector
   StoreGateSvc* detStore(0);
   const MuonGM::MuonDetectorManager* muonMgr;
   sc = serviceLocator()->service("DetectorStore", detStore);
   if (sc.isFailure()) {
     ATH_MSG_ERROR("Could not retrieve DetectorStore.");
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved DetectorStore.");
   
   sc = detStore->retrieve( muonMgr,"Muon" );
   if (sc.isFailure()) return sc;
   ATH_MSG_DEBUG("Retrieved GeoModel from DetectorStore.");
   m_mdtIdHelper = muonMgr->mdtIdHelper();
   
   // Locate RegionSelector
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
     ATH_MSG_ERROR("Could not retrieve the regionselector service");
      return sc;
   }
   ATH_MSG_DEBUG("Retrieved the RegionSelector service ");

   if (m_use_rpc) {
     sc = m_rpcDataPreparator.retrieve();
     if ( sc.isFailure() ) {
       ATH_MSG_ERROR("Could not retrieve " << m_rpcDataPreparator);
       return sc;
     }
     ATH_MSG_DEBUG("Retrieved service " << m_rpcDataPreparator);
   }

   sc = m_tgcDataPreparator.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR("Could not retrieve " << m_tgcDataPreparator);
      return sc;
   }
   ATH_MSG_DEBUG("Retrieved service " << m_tgcDataPreparator);

   sc =m_mdtDataPreparator.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR("Could not retrieve " << m_mdtDataPreparator);
      return sc;
   }
   ATH_MSG_DEBUG("Retrieved service " << m_mdtDataPreparator);
   
   sc =m_cscDataPreparator.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR("Could not retrieve " << m_cscDataPreparator);
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved service " << m_cscDataPreparator);

   sc =m_rpcRoadDefiner.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR("Could not retrieve " << m_rpcRoadDefiner);
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved service " << m_rpcRoadDefiner);

   sc =m_tgcRoadDefiner.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR("Could not retrieve " << m_tgcRoadDefiner);
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved service " << m_tgcRoadDefiner);

   sc =m_rpcPatFinder.retrieve();
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR("Could not retrieve " << m_rpcPatFinder);
     return sc;
   }
   ATH_MSG_DEBUG("Retrieved service " << m_rpcPatFinder);

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
    ATH_MSG_ERROR("Could not find PtEndcapLUTSvc");
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

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastDataPreparator::setRpcGeometry(bool use_rpc)
{
  m_rpcRoadDefiner->setRpcGeometry(use_rpc);
  m_mdtDataPreparator->setRpcGeometry(use_rpc);

  m_use_rpc = use_rpc;
  return;
}

// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastDataPreparator::setMdtDataCollection(bool use_mdtcsm)
{
  m_mdtDataPreparator->setMdtDataCollection(use_mdtcsm);
  return;
}

// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastDataPreparator::setRoIBasedDataAccess(bool use_RoIBasedDataAccess_MDT,
                                                               bool use_RoIBasedDataAccess_RPC,
                                                               bool use_RoIBasedDataAccess_TGC,
                                                               bool use_RoIBasedDataAccess_CSC)
{                               
  m_mdtDataPreparator->setRoIBasedDataAccess(use_RoIBasedDataAccess_MDT);
  m_rpcDataPreparator->setRoIBasedDataAccess(use_RoIBasedDataAccess_RPC);
  m_tgcDataPreparator->setRoIBasedDataAccess(use_RoIBasedDataAccess_TGC);
  m_cscDataPreparator->setRoIBasedDataAccess(use_RoIBasedDataAccess_CSC);
  return;
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
                                                           const TrigRoiDescriptor*    p_roids,
                                                           TrigL2MuonSA::RpcHits&      rpcHits,
                                                           TrigL2MuonSA::MuonRoad&     muonRoad,
                                                           TrigL2MuonSA::MdtRegion&    mdtRegion,
                                                           TrigL2MuonSA::RpcFitResult& rpcFitResult,
                                                           TrigL2MuonSA::MdtHits&      mdtHits_normal,
                                                           TrigL2MuonSA::MdtHits&      mdtHits_overlap)
{

  ATH_MSG_DEBUG("RoI eta/phi=" << p_roi->eta() << "/" << p_roi->phi());  
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if(!m_use_rpc){

  } else {
    
    m_rpcPatFinder->clear();

    unsigned int roiWord = p_roi->roiWord();
    sc = m_rpcDataPreparator->prepareData(p_roids,
					  roiWord,
                                          rpcHits,
                                          &m_rpcPatFinder);

    // check if the RoI is fake and se the flag
    m_isRpcFakeRoi = m_rpcDataPreparator->isFakeRoi();

    if (!sc.isSuccess()) {
      ATH_MSG_DEBUG("Error in RPC data prepapration. Continue using RoI");
    }
  }
 
  m_recRPCRoiSvc->reconstruct(p_roi->roiWord());
  double roiEtaMinLow = 0.;
  double roiEtaMaxLow = 0.;
  double roiEtaMinHigh = 0.;
  double roiEtaMaxHigh = 0.;
  m_recRPCRoiSvc->etaDimLow(roiEtaMinLow, roiEtaMaxLow);
  m_recRPCRoiSvc->etaDimHigh(roiEtaMinHigh, roiEtaMaxHigh);

  sc = m_rpcRoadDefiner->defineRoad(p_roi,
                                    muonRoad,
                                    rpcHits,
                                    &m_rpcPatFinder,
                                    rpcFitResult,
                                    roiEtaMinLow,
                                    roiEtaMaxLow,
                                    roiEtaMinHigh,
                                    roiEtaMaxHigh);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Error in road definition.");
    return sc;
  }
  
  sc = m_mdtDataPreparator->prepareData(p_roi,
                                        p_roids,
                                        rpcFitResult,
                                        muonRoad,
                                        mdtRegion,
                                        mdtHits_normal,
                                        mdtHits_overlap);
  
  
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Error in MDT data preparation.");
    return sc;
  }
  ATH_MSG_DEBUG("nr of MDT (normal)  hits=" << mdtHits_normal.size());
  ATH_MSG_DEBUG("nr of MDT (overlap) hits=" << mdtHits_overlap.size());
  
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastDataPreparator::prepareData(const LVL1::RecMuonRoI*     p_roi,
                                                           const TrigRoiDescriptor*    p_roids,
                                                           TrigL2MuonSA::TgcHits&      tgcHits,
                                                           TrigL2MuonSA::MuonRoad&     muonRoad,
                                                           TrigL2MuonSA::MdtRegion&    mdtRegion,
                                                           TrigL2MuonSA::TgcFitResult& tgcFitResult,
                                                           TrigL2MuonSA::MdtHits&      mdtHits_normal,
                                                           TrigL2MuonSA::MdtHits&      mdtHits_overlap,
                                                           TrigL2MuonSA::CscHits&      cscHits)
{
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG("RoI eta/phi=" << p_roi->eta() << "/" << p_roi->phi());
  
  sc = m_tgcDataPreparator->prepareData(p_roi,
                                        tgcHits);
  if (!sc.isSuccess()) {
    ATH_MSG_DEBUG("Error in TGC data preparation. Continue using RoI");
  }
  ATH_MSG_DEBUG("nr of TGC hits=" << tgcHits.size());
  
  sc = m_tgcRoadDefiner->defineRoad(p_roi,
                                    tgcHits,
                                    muonRoad,
                                    tgcFitResult);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Error in road definition.");
    return sc;
  }
  
  sc = m_mdtDataPreparator->prepareData(p_roi,
                                        p_roids,
                                        tgcFitResult,
                                        muonRoad,
                                        mdtRegion,
                                        mdtHits_normal,
                                        mdtHits_overlap);

  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Error in MDT data preparation.");
    return sc;
  }
  ATH_MSG_DEBUG("nr of MDT (normal)  hits=" << mdtHits_normal.size());
  ATH_MSG_DEBUG("nr of MDT (overlap) hits=" << mdtHits_overlap.size());
  
  sc = m_cscDataPreparator->prepareData(p_roids,
                                        muonRoad,
                                        cscHits);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Error in CSC data preparation.");
    return sc;
  }
  ATH_MSG_DEBUG("nr of CSC hits=" << cscHits.size());

  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastDataPreparator::finalize()
{
  ATH_MSG_DEBUG("Finalizing MuFastDataPreparator - package version " << PACKAGE_VERSION);
   
   StatusCode sc = AthAlgTool::finalize(); 
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not finalize the AthAlgTool base class.");
     return sc;
   }
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
