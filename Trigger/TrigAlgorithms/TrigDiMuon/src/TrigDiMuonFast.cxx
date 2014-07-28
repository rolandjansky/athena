/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*----------------------------------------
        Authors: Shlomit Tarem
                 Natasha Panikashvili
                 Daniel Scheirich
------------------------------------------*/
//
// Monitoring : Shlomit Tarem ,Natasha Panikashvili  and Nir Guttman
//
#include "TrigDiMuon/TrigDiMuonFast.h"
#include "TrigDiMuon/Candidate.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "RegionSelector/IRegSelSvc.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "MuonRDO/RpcPadContainer.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/RpcPadIdHash.h"
#include "RPCcablingInterface/CablingRPCBase.h"
#include "RPCgeometry/RPCGeometry.h"
#include "TrigT1RPClogic/RPCrawDigit.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "PathResolver/PathResolver.h"

#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"
#include "TrigVertexFitter/ITrigVertexingTool.h"

#include "MuonContainerManager/MuonRdoContainerAccess.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
//////#include "TrigMuonEvent/MuonFeatureDetails.h"


#include <iostream>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <vector>

const double FIFTEEN_DEG    = 15.0 * M_PI / 180.0;
const double FORTY_FIVE_DEG = 45.0 * M_PI / 180.0;
const double PI_OVER_4      = M_PI/4.;
const double PI_OVER_8      = M_PI/8.;
Identifier tgc_rdo_id_previous;
std::ofstream save_to_file;

TrigDiMuonFast::TrigDiMuonFast(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::FexAlgo(name, pSvcLocator),

  m_rpc_converter("Muon__RPC_RawDataProviderTool"),
  m_mdt_converter("Muon__MDT_RawDataProviderTool"),
  m_tgc_converter("Muon__TGC_RawDataProviderTool"),
//   m_csc_converter("Muon__CSC_RawDataProviderTool"),
  m_extrapolator("TrigDiMuonExtrapolator"),
  m_pMuonMgr          (NULL),
  m_pTgcIdHelper      (NULL),
  m_pTgcLUTSvc        (NULL),
  m_pRegionSelector   (NULL),
  m_pMuonRoI          (NULL),
  m_pStoreGate        (NULL),
  m_RPCgeometrySvc    ("RPCgeometrySvc",""),
  m_TGCgeometrySvc    ("TGCgeometrySvc",""),
  m_MDTcablingSvc     ("MDTcablingSvc",""),
  m_L2vertFitter("TrigL2VertexFitter",this),
  m_vertexingTool("TrigVertexingTool",this)

{

  //tools
  declareProperty("Extrapolator", m_extrapolator);
  declareProperty( "RPC_RawDataProvider", m_rpc_converter);
  declareProperty( "MDT_RawDataProvider", m_mdt_converter);
  declareProperty( "TGC_RawDataProvider", m_tgc_converter);
//   declareProperty( "CSC_RawDataProvider", m_csc_converter);
  declareProperty( "RPC_Geometry", m_RPCgeometrySvc);
  declareProperty( "TGC_Geometry", m_TGCgeometrySvc);
  declareProperty( "MDT_Cabling", m_MDTcablingSvc);

  declareProperty("TrigL2VertexFitter", m_L2vertFitter);
  declareProperty("TrigVertexingTool" , m_vertexingTool, "TrigVertexingTool");

  declareProperty("MdtDigits_PhiCut",     m_MdtDigits_PhiCut=0.1);
  declareProperty("MdtDigits_EtaCut",     m_MdtDigits_EtaCut=0.06);
  declareProperty("MdtDelta_PhiCut",      m_MdtDelta_PhiCut=0.2);
  declareProperty("MdtDelta_EtaCut",      m_MdtDelta_EtaCut=0.06);


  declareProperty("RpcDigits_PhiCut",     m_RpcDigits_PhiCut=0.1);
  declareProperty("RpcDigits_EtaCut",     m_RpcDigits_EtaCut=0.1);
  declareProperty("RpcPhiDelta_PhiCut",   m_RpcPhiDelta_PhiCut=0.1);
  declareProperty("RpcPhiDelta_EtaCut",   m_RpcPhiDelta_EtaCut=0.2);
  declareProperty("RpcEtaDelta_PhiCut",   m_RpcEtaDelta_PhiCut=0.2);
  declareProperty("RpcEtaDelta_EtaCut",   m_RpcEtaDelta_EtaCut=0.1);
  declareProperty("RpcEtaFactor",         m_RpcEtaFactor  = 2.);
  declareProperty("RpcPhiFactor",         m_RpcPhiFactor  = 2.);
  declareProperty("TgcEtaFactor",         m_TgcEtaFactor  = 2.);
  declareProperty("TgcPhiFactor",         m_TgcPhiFactor  = 2.);
  declareProperty("MdtEtaFactor",         m_MdtEtaFactor  = 2.);
  declareProperty("JpsiMassMin",          m_JpsiMassMin   = 500.);
  declareProperty("PtMin",                m_PtMin   = 2200.);
  declareProperty("PHI_DELTA_TGC",        m_PHI_DELTA_TGC = 0.1);
  declareProperty("ETA_DELTA_TGC",        m_ETA_DELTA_TGC = 0.1);
  declareProperty("doLutTgcRdo",          m_doLutTgcRdo   = true);
  declareProperty("UseTiltedBField",      m_tilted_B_field= false);
  declareProperty("NHits",                m_NHits  =  3);

  declareProperty("doFillESD",            m_fill_esd    = true);
  declareProperty("ESD_EXT_size",         m_esd_ext_size=10);
  declareProperty("ESD_ROB_size",         m_esd_rob_size=0);
  declareProperty("ESD_CSM_size",         m_esd_csm_size=0);
  declareProperty("ESD_LV1_size",         m_esd_lv1_size=0);
  declareProperty("ESD_RPC_size",         m_esd_rpc_size=100);
  declareProperty("ESD_TGC_size",         m_esd_tgc_size=50);
  declareProperty("ESD_MDT_size",         m_esd_mdt_size=100);

  declareProperty("doTagAndProbe",        m_doTagAndProbe     = true);
  declareProperty("doOppChargeCheck",     m_doOppChargeCheck  = false);
  declareProperty("doVertexing",          m_doVertexing       = true);
  declareProperty("doRoiCheck",           m_doRoiCheck        = false);

  declareProperty("roiEndcap_dEtaCut",    m_roiEndcap_dEtaCut = 0.2);
  declareProperty("roiEndcap_dPhiCut",    m_roiEndcap_dPhiCut = 0.2);
  declareProperty("roiBarrel_dEtaCut",    m_roiBarrel_dEtaCut = 0.2);
  declareProperty("roiBarrel_dPhiCut",    m_roiBarrel_dPhiCut = 0.2);

  declareProperty("retrieveRoI",          m_retrieveRoI = true);

  //Monitoring Var

  declareMonitoredStdContainer("PMuonBarrelMdtHits", m_pos_muon_barrel_mdt_hits);
  declareMonitoredStdContainer("NMuonBarrelMdtHits", m_neg_muon_barrel_mdt_hits);
  declareMonitoredStdContainer("PMuonCapMdtHits", m_pos_muon_cap_mdt_hits);
  declareMonitoredStdContainer("NMuonCapMdtHits", m_neg_muon_cap_mdt_hits);
  declareMonitoredStdContainer("PMuonTgcHits",m_pos_muon_tgc_hits);
  declareMonitoredStdContainer("PMuonRpcHits",m_pos_muon_rpc_hits);
  declareMonitoredStdContainer("NMuonTgcHits",m_neg_muon_tgc_hits);
  declareMonitoredStdContainer("NMuonRpcHits",m_neg_muon_rpc_hits);

  declareMonitoredVariable("JPsiPerRoi",m_j_psi_per_roi);

  declareMonitoredStdContainer("JPsiMass",m_j_psi_mass);
  declareMonitoredStdContainer("JPsiMassWideRange",m_j_psi_mass_wide);
  declareMonitoredStdContainer("JPsiEta",m_j_psi_eta);
  declareMonitoredStdContainer("JPsiPhi", m_j_psi_phi);
  declareMonitoredStdContainer("JPsiPt",m_j_psi_pt);
  declareMonitoredStdContainer("JPsiPtWideRange",m_j_psi_pt);
  declareMonitoredStdContainer("JPsiChi2",m_j_psi_chi2);
  declareMonitoredStdContainer("JPsiLifeTime",m_j_psi_lifetime);
  declareMonitoredStdContainer("PMuonEta",m_pos_muon_eta);
  declareMonitoredStdContainer("PMuonPhi",m_pos_muon_phi);
  declareMonitoredStdContainer("PMuonPt",m_pos_muon_pt);
  declareMonitoredStdContainer("NMuonEta",m_neg_muon_eta);
  declareMonitoredStdContainer("NMuonPhi", m_neg_muon_phi);
  declareMonitoredStdContainer("NMuonPt", m_neg_muon_pt);

  declareMonitoredStdContainer("RoiEndcap_dEta",  m_roiEndcap_dEta);
  declareMonitoredStdContainer("RoiEndcap_dPhi",  m_roiEndcap_dPhi);
  declareMonitoredStdContainer("RoiEndcap_dR",    m_roiEndcap_dR);
  declareMonitoredStdContainer("RoiBarrel_dEta",  m_roiBarrel_dEta);
  declareMonitoredStdContainer("RoiBarrel_dPhi",  m_roiBarrel_dPhi);
  declareMonitoredStdContainer("RoiBarrel_dR",    m_roiBarrel_dR);

  declareMonitoredStdContainer("RoiEndcapNoEx_dEta",  m_roiEndcap_ne_dEta);
  declareMonitoredStdContainer("RoiEndcapNoEx_dPhi",  m_roiEndcap_ne_dPhi);
  declareMonitoredStdContainer("RoiEndcapNoEx_dR",    m_roiEndcap_ne_dR);
  declareMonitoredStdContainer("RoiBarrelNoEx_dEta",  m_roiBarrel_ne_dEta);
  declareMonitoredStdContainer("RoiBarrelNoEx_dPhi",  m_roiBarrel_ne_dPhi);
  declareMonitoredStdContainer("RoiBarrelNoEx_dR",    m_roiBarrel_ne_dR);

  declareMonitoredStdContainer("roiNoHits_pt",     m_roiNoHits_pt);
  declareMonitoredStdContainer("roiNoHits_charge", m_roiNoHits_charge);
  declareMonitoredStdContainer("roiNoHits_eta",    m_roiNoHits_eta);
  declareMonitoredStdContainer("roiNoHits_phi",    m_roiNoHits_phi);

  declareMonitoredStdContainer("TagAndProbe_trTr",    m_tap_mass_trTr    );
  declareMonitoredStdContainer("TagAndProbe_trTr_OS", m_tap_mass_trTr_os );
  declareMonitoredStdContainer("TagAndProbe_trRoI",   m_tap_mass_trRoI   );
  declareMonitoredStdContainer("TagAndProbe_muRoI",   m_tap_mass_muRoI   );
  declareMonitoredStdContainer("TagAndProbe_muMuRoI", m_tap_mass_muMuRoI );

  declareMonitoredVariable("Counter_allTrk", m_cnt_allTrk );
  declareMonitoredVariable("Counter_accTrk", m_cnt_accTrk );
  declareMonitoredVariable("Counter_trTr", m_cnt_trTr );
  declareMonitoredVariable("Counter_muMu", m_cnt_muMu );
  declareMonitoredVariable("Counter_muMuRoI", m_cnt_muMuRoI );
  declareMonitoredVariable("Counter_muMuVtx", m_cnt_muMuVtx );
  declareMonitoredVariable("Counter_muMuRoIVtx", m_cnt_muMuRoIVtx );

  declareMonitoredStdContainer("Errors", m_hist_error);
  declareMonitoredStdContainer("Debug", m_hist_debug);
  declareMonitoredStdContainer("Acceptance", m_hist_lastStage);
  declareMonitoredStdContainer("Settings", m_hist_settings);

  declareMonitoredVariable("Roi_eta", m_roi_eta);
  declareMonitoredVariable("Roi_phi", m_roi_phi);

  declareMonitoredStdContainer("Hit_Endcap_mdt_inner_dEta" , m_hit_Endcap_mdt_inner_dEta );
  declareMonitoredStdContainer("Hit_Endcap_mdt_middle_dEta", m_hit_Endcap_mdt_middle_dEta);
  declareMonitoredStdContainer("Hit_Barrel_mdt_inner_dEta" , m_hit_Barrel_mdt_inner_dEta );
  declareMonitoredStdContainer("Hit_Barrel_mdt_middle_dEta", m_hit_Barrel_mdt_middle_dEta);
  declareMonitoredStdContainer("Hit_rpc_middle_dEta"       , m_hit_rpc_middle_dEta       );
  declareMonitoredStdContainer("Hit_rpc_middle_dPhi"       , m_hit_rpc_middle_dPhi       );
  declareMonitoredStdContainer("Hit_tgc_wire_inner_dEta"   , m_hit_tgc_wire_inner_dEta   );
  declareMonitoredStdContainer("Hit_tgc_wire_middle_dEta"  , m_hit_tgc_wire_middle_dEta  );
  declareMonitoredStdContainer("Hit_tgc_wire_inner_dPhi"   , m_hit_tgc_wire_inner_dPhi   );
  declareMonitoredStdContainer("Hit_tgc_wire_middle_dPhi"  , m_hit_tgc_wire_middle_dPhi  );
  declareMonitoredStdContainer("Hit_tgc_strip_inner_dPhi"  , m_hit_tgc_strip_inner_dPhi  );
  declareMonitoredStdContainer("Hit_tgc_strip_middle_dPhi" , m_hit_tgc_strip_middle_dPhi );

  declareMonitoredStdContainer("vtx_mass" , m_vtx_mass);
  declareMonitoredStdContainer("vtx_massWideRange" , m_vtx_mass);
  declareMonitoredStdContainer("vtx_chi2" , m_vtx_chi2);
  declareMonitoredStdContainer("vtx_ndof" , m_vtx_ndof);
  declareMonitoredStdContainer("vtx_x"    , m_vtx_x   );
  declareMonitoredStdContainer("vtx_y"    , m_vtx_y   );
  declareMonitoredStdContainer("vtx_z"    , m_vtx_z   );

}

TrigDiMuonFast::~TrigDiMuonFast()
{
}

HLT::ErrorCode TrigDiMuonFast::hltInitialize()
{
  msg()<< MSG::INFO << " Initialize TrigDiMuon " << endreq;

  StatusCode status;

  //initialize extrapolator
  status = m_extrapolator.retrieve();
  if ( status.isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve TrigDiMuonExtrapolator!" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // initialize the new converters
  status = m_rpc_converter.retrieve();
  if (status.isFailure())
  {
    msg() << MSG::ERROR << "Could not retrieve "
        << m_rpc_converter << endreq;
    return HLT::BAD_JOB_SETUP;;
  }
  else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_rpc_converter << endreq;
  }

  status = m_mdt_converter.retrieve();
  if (status.isFailure())
  {
    msg() << MSG::ERROR << "Could not retrieve "
        << m_mdt_converter << endreq;
    return HLT::BAD_JOB_SETUP;;
  }
  else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_mdt_converter << endreq;
  }

  status = m_tgc_converter.retrieve();
  if (status.isFailure())
  {
    msg() << MSG::ERROR << "Could not retrieve "
        << m_tgc_converter << endreq;
    return HLT::BAD_JOB_SETUP;;
  }
  else
  {
    msg() << MSG::INFO << "Retrieved tool " << m_tgc_converter << endreq;
  }

//   status = m_csc_converter.retrieve();
//   if (status.isFailure())
//   {
//     msg() << MSG::ERROR << "Could not retrieve "
//         << m_csc_converter << endreq;
//     return HLT::BAD_JOB_SETUP;;
//   }
//   else
//   {
//     msg() << MSG::INFO << "Retrieved tool " << m_csc_converter << endreq;
//   }


    // initialize MuonDetectorManager
  StoreGateSvc* pDetStore = NULL;
  status = serviceLocator()->service("DetectorStore", pDetStore);
  if (status.isFailure() || pDetStore == NULL)
  {
    msg()<< MSG::ERROR << "Cannot locate DetectorStore" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  status = pDetStore->retrieve(m_pMuonMgr);
  if (status.isFailure() || m_pMuonMgr == NULL)
  {
    msg()<< MSG::ERROR << "Cannot retrieve MuonDetectorManager" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  m_pTgcIdHelper = m_pMuonMgr->tgcIdHelper();

    // initialize RegionSelector
  status = service("RegSelSvc", m_pRegionSelector);
  if (status.isFailure())
  {
    msg()<< MSG::ERROR << " Cannot initialize RegionSelector tool " << endreq;
    return HLT::BAD_JOB_SETUP;
  }

    // get MDT cabling service
  status = m_MDTcablingSvc.retrieve();
  if (status.isFailure())
  {
    msg()<< MSG::ERROR << "Cannot retrieve MDT cabling Service" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

    // get RPC cabling service
  const IRPCcablingServerSvc* pRpcCabGet = NULL;
  status = serviceLocator()->service("RPCcablingServerSvc", pRpcCabGet);
  if (status.isFailure() || pRpcCabGet == NULL)
  {
    msg()<< MSG::ERROR << " Cannot retrieve RPC cabling server" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  if (StatusCode::SUCCESS != pRpcCabGet->giveCabling(pRpcCabling))
  {
    msg()<< MSG::ERROR << " Cannot retrieve RPC cabling service from server" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  status = m_RPCgeometrySvc.retrieve();
  if (status.isFailure())
  {
    msg()<< MSG::ERROR << "Cannot retrieve RPC geometry Service" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  m_pRpcGeometry = 0;

  status = m_TGCgeometrySvc.retrieve();
  if (status.isFailure())
  {
    msg()<< MSG::ERROR << "Cannot retrieve TGC geometry Service" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  m_pTgcLUTSvc = &(*m_TGCgeometrySvc);

  // Retrieve the vertexing tools

  status = m_L2vertFitter.retrieve();
  if ( status.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  else {
    msg() << MSG::INFO << "TrigL2VertexFitter retrieved" << endreq;
  }

  status = m_vertexingTool.retrieve();
  if ( status.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigVertexingTool tool" << endreq;
    return HLT::BAD_JOB_SETUP;
  } else {
    msg() << MSG::INFO << "TrigVertexingTool retrieved" << endreq;
  }

  //timing
  if (doTiming())
  {
    m_timerTotal     = addTimer("Total");       // time of all executable
    m_timerTotal1    = addTimer("TotalExFun");  // time of all algorithm except function
    m_timerPerFun    = addTimer("PerFunction"); // time per function
    m_timerAllFun    = addTimer("AllFun");      // function time (all functions together)
    m_timerTotalMdt  = addTimer("TotalMdt");    // time of all MDT part
    m_timerTotalRpc  = addTimer("TotalRpc");    // all RPC part
    m_timerRpcProp   = addTimer("RpcProp");     // RPC propagation
    m_timerRpcLUT    = addTimer("RpcLUT");      // RPC LUT
    m_timerRpcData   = addTimer("RpcData");     // RPC data taking
    m_timerRpcPoints = addTimer("RpcPoints");   // prepare RPC points
    m_timerTotalTgc  = addTimer("TotalTgc");    // all TGC part
    m_timerTgcProp   = addTimer("TgcProp");     // TGC propagation
    m_timerTgcLUT    = addTimer("TgcLUT");     // TGC LUT rdo
    m_timerTgcPoints = addTimer("TgcPoints");   // prepare TGC points
  }


  IService* svc ;
  if (StatusCode::SUCCESS != service("ROBDataProviderSvc",svc))
  {
    msg() << MSG::ERROR << " Can't get ROBDataProviderSvc " << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  m_pROBDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
  if (m_pROBDataProvider == 0)
  {
    msg() << MSG::ERROR << " Cant cast to TrigL2ROBDataProviderSvc " <<endreq;
    return HLT::BAD_JOB_SETUP ;
  }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Algorithm TrigDiMuon initialized successfully" << endreq;
  return HLT::OK;

}

//bool CompPtGreater(const TrigInDetTrack* tr1, const TrigInDetTrack* tr2)
bool CompPtGreater(const std::pair<TrigInDetTrack*, bool> & elem1,
                   const std::pair<TrigInDetTrack*, bool> & elem2)
{
    //std::cout << "first elem 1 " << fabs(elem1.first->param()->pT()) << std::endl;
    //std::cout << "first elem 2 " << fabs(elem2.first->param()->pT()) << std::endl;
    //return fabs(tr1->param()->pT()) < fabs(tr2->param()->pT());
  return (fabs(elem1.first->param()->pT()) > fabs(elem2.first->param()->pT()));
}


HLT::ErrorCode TrigDiMuonFast::hltExecute(const HLT::TriggerElement* inputTE,
                                          HLT::TriggerElement* outputTE)
{

  //monitoring

  //clean up histograms
  m_hist_error.clear();
  m_hist_debug.clear();
  m_hist_lastStage.clear();
  m_hist_cand_lastStage.clear();

  m_hist_settings.clear();

  //fill settings histogram
  if(m_doLutTgcRdo) m_hist_settings.push_back(TrigDiMuonSetings::doLutTgcRdo);
  if(m_tilted_B_field) m_hist_settings.push_back(TrigDiMuonSetings::UseTiltedBField);
  if(m_fill_esd) m_hist_settings.push_back(TrigDiMuonSetings::doFillESD);
  if(m_doTagAndProbe) m_hist_settings.push_back(TrigDiMuonSetings::doTagAndProbe);
  if(m_doOppChargeCheck) m_hist_settings.push_back(TrigDiMuonSetings::doOppChargeCheck);
  if(m_doVertexing) m_hist_settings.push_back(TrigDiMuonSetings::doVertexing);
  if(m_doRoiCheck) m_hist_settings.push_back(TrigDiMuonSetings::doRoiCheck);
  if(doTiming()) m_hist_settings.push_back(TrigDiMuonSetings::doTiming);
  if(m_retrieveRoI) m_hist_settings.push_back(TrigDiMuonSetings::retrieveRoI);


  m_hist_lastStage.push_back(TrigDiMuonAcc::input);  //last stage of the algorithm executed

  m_pStoreGate = store();

  // Retrieve the EventInfo Objects //////////////////////////////////////////
  const EventInfo* pEvent(0);                                               //
  StatusCode status = m_pStoreGate->retrieve(pEvent);                       //
  if (status.isFailure())                                               //
  {
    msg() << MSG::ERROR << "Could not find EventInfo object" << endreq;   //
    return HLT::SG_ERROR;                                                 //
  }                                                                         //
  //
  const EventID* pEventId = pEvent->event_ID();                             //
  if (pEventId==0)                                                         //
  {
    msg() << MSG::ERROR << "Could not find EventID object" << endreq;     //
    m_hist_error.push_back(TrigDiMuonErr::noEventInfo);
    return HLT::SG_ERROR;                                                 //
  }                                                                         //
  m_current_run_id = pEventId->run_number();                                //
  m_current_lbk_id = pEventId->lumi_block();                                //
  m_current_bcg_id = pEventId->bunch_crossing_id();                         //
  //
  const TriggerInfo* pTriggerInfo = pEvent->trigger_info();                 //
  if (pTriggerInfo==0)                                                     //
  {
    m_hist_error.push_back(TrigDiMuonErr::noTriggerInfo);
    msg() << MSG::ERROR << "Could not find TriggerInfo object" << endreq; //
    return HLT::SG_ERROR;                                                 //
  }                                                                         //
  //
  //if(m_detector_mask_not_checked &&                                         //
  //      m_detector_mask_on_event) setup_detector_mask(pEventId);            //
  //
  m_current_lvl1_id = pTriggerInfo->extendedLevel1ID();                     //
  //
  if (msgLvl() <= MSG::DEBUG)                                              //
  {
    msg() << MSG::DEBUG << " ---> Run Number       : "                    //
        << m_current_run_id << endreq;                    //
    msg() << MSG::DEBUG << " ---> Luminosity Block : " << std::hex        //
        << m_current_lbk_id << std::dec << endreq;        //
    msg() << MSG::DEBUG << " ---> Bunch Crossing ID: " << std::hex        //
        << m_current_bcg_id << std::dec << endreq;        //
    msg() << MSG::DEBUG << " ---> Extended Lvl1 ID : " << std::hex        //
        << m_current_lvl1_id << std::dec << endreq;       //
  }                                                                         //
  ////////////////////////////////////////////////////////////////////////////

  if (doTiming())
  {
    m_timerTotal->start();
    m_timerTotal1->start();
    m_timerAllFun->start();
    m_timerAllFun->pause();
  }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Executing HLT TrigDiMuonFast: inputTE->getId= " << inputTE->getId()  << endreq;

    //StatusCode status = StatusCode::SUCCESS;

  std::auto_ptr<TrigL2BphysContainer> trigBphysColl =
      std::auto_ptr<TrigL2BphysContainer>(new TrigL2BphysContainer());

  std::auto_ptr<TrigVertexCollection> trigVertexColl =
      std::auto_ptr<TrigVertexCollection>(new TrigVertexCollection());

  if(m_retrieveRoI) {
    // get vector of input TE
    std::vector<const LVL1::RecMuonRoI*> vectorOfRecMuonRoI;
    HLT::ErrorCode sc = getFeatures(inputTE, vectorOfRecMuonRoI, "");
    if (sc!=HLT::OK)
    {
      m_hist_error.push_back(TrigDiMuonErr::noRoI);
      msg() << MSG::ERROR << " Cannot get vector of vectorOfRecMuonRoI " << sc << endreq;
          //delete di_muon;
      return HLT::NAV_ERROR;
    }

    if (vectorOfRecMuonRoI.size() != 1)
    {
      m_hist_error.push_back(TrigDiMuonErr::moreThanOneRoI);
      msg() << MSG::ERROR  << "Size of vector is not 1 but " << vectorOfRecMuonRoI.size() << endreq;
          //delete di_muon;
      return HLT::NAV_ERROR;
    }

      //unsigned int currentLvl1ID = 0;

      // Get the Lvl1 Id to intrumet the NetLoger calls
      /*
    const EventInfo* constEventInfo(0);
    StatusCode status = m_pStoreGate->retrieve(constEventInfo);
    if (status.isFailure())
    {
    msg() << "Can't get EventInfo object for update the Lvl1Result"
    << endreq;
    return HLT::SG_ERROR;
  }
    else
    {
    currentLvl1ID = constEventInfo->event_ID()->event_number();
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " ---> Lvl1Result value " << currentLvl1ID << endreq;
  }
      */

      //Get first (and only) ROI
    m_pMuonRoI = vectorOfRecMuonRoI.front();
    if (m_pMuonRoI == NULL)
    {
      m_hist_error.push_back(TrigDiMuonErr::noRoI);
      msg() << MSG::ERROR << "Retrieval of RoI from vector failed" << endreq;
          //delete di_muon;
      return HLT::NAV_ERROR;
    }

    m_hist_lastStage.push_back(TrigDiMuonAcc::gotRoI);   //last stage of the algorithm executed

    //monitoring
    m_roi_eta = m_pMuonRoI->eta();
    m_roi_phi = m_pMuonRoI->phi();

  }


  //retrive MDTCSM container
  pMdtContainer = Muon::MuonRdoContainerAccess::retrieveMdtCsm("MDTCSM");
  if (pMdtContainer==0)
  {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "MDT CSM container not registered by "
          << "MuonRdoContainerManager; retrieving it "
          << "from the store! " << endreq;

    status = m_pStoreGate->retrieve(pMdtContainer, "MDTCSM");
    if (status.isFailure())
    {
      m_hist_error.push_back(TrigDiMuonErr::noMdtCont);
      msg() << MSG::ERROR << "Retrieval of MdtCsmContainer failed"
          << endreq;
      return HLT::SG_ERROR;
    }
  }

  //retrive RpcPadContainer
  m_pRpcPadContainer = Muon::MuonRdoContainerAccess::retrieveRpcPad("RPCPAD");
  if (m_pRpcPadContainer==0)
  {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Rpc PAD container not registered by "
          << "MuonRdoContainerManager; retrieving it "
          << "from the store! " << endreq;

    status = m_pStoreGate->retrieve(m_pRpcPadContainer, "RPCPAD");
    if (status.isFailure())
    {
      m_hist_error.push_back(TrigDiMuonErr::noRpcCont);
      msg() << MSG::ERROR << "Retrieval of RpcPadContainer failed"
          << endreq;
      return HLT::SG_ERROR;
    }
  }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Retrieved " << m_pRpcPadContainer->size() << " RPC RDOs." << endreq;

    //retrive TgcRdoContainer
  pTgcRdoContainer = Muon::MuonRdoContainerAccess::retrieveTgcRdo("TGCRDO");
  if (pTgcRdoContainer==0)
  {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Tgc RDO container not registered by "
          << "MuonRdoContainerManager; retrieving it "
          << "from the store! " << endreq;

    status = m_pStoreGate->retrieve(pTgcRdoContainer, "TGCRDO");
    if (status.isFailure())
    {
      m_hist_error.push_back(TrigDiMuonErr::noTgcCont);
      msg() << MSG::ERROR << "Retrieval of TgcRdoContainer failed"
          << endreq;
      return HLT::SG_ERROR;
    }
  }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "TgcRdoContainer size=" << pTgcRdoContainer->size() << endreq;
  tgc_rdo_id_previous.clear();

  //OPEN InDetTrackCollection

  std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
  status = getFeatures(outputTE,vectorOfTrackCollections, "");

  if (status != HLT::OK)
  {
    m_hist_error.push_back(TrigDiMuonErr::noTrackColl);
    msg() << MSG::ERROR << " Cannot get InDetTrackCollections " << endreq;
        //delete di_muon;
    return HLT::SG_ERROR;
  }

  if(vectorOfTrackCollections.size()==0)
    m_hist_debug.push_back(TrigDiMuonDebug::trkCollEmpty);  //debug monitoring

  if(vectorOfTrackCollections.size()>1)
    m_hist_debug.push_back(TrigDiMuonDebug::moreTrkColl);  //debug monitoring

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() << " InDetTrackCollections, now fill InDet Block " << endreq;

  std::vector<std::pair<TrigInDetTrack*, TrigInDetTrack*> > PairCandidate;
  std::vector<const TrigInDetTrackCollection*>::iterator
      theTrackColl = vectorOfTrackCollections.begin(),
  endTrackColl = vectorOfTrackCollections.end();
  for (;theTrackColl != endTrackColl;  theTrackColl++)
  {

    if((*theTrackColl)->size()==0)
      m_hist_debug.push_back(TrigDiMuonDebug::trkCollEmpty);  //debug monitoring

    if(m_hist_lastStage.back() < TrigDiMuonAcc::gotTracks ) // last stage of the algorithm executed
      m_hist_lastStage.push_back(TrigDiMuonAcc::gotTracks);

    processTrackColl(*theTrackColl, trigBphysColl.get(), trigVertexColl.get(), m_pMuonRoI, inputTE, outputTE);
  }


  //   Store it in StoreGate using the original Key2Key method (optional)
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " di_muon->size() = " << trigBphysColl->size() << endreq;

//    if (msgLvl() <= MSG::DEBUG)
//        msg() << MSG::DEBUG << m_tgc_lut_digits.size() << " TGC hits "
//        << m_rpc_raw_data_list.size() << " RPC hits "
//        << m_mdt_tubes.size() << " MDT hits "
//        << endreq;

  if (trigBphysColl->size()>0)
  {
    if (msgLvl() <= MSG::DEBUG)
      msg()  << MSG::DEBUG << " Store TrigBphys Collection size: " << trigBphysColl->size() << endreq;

    std::string Key = "";
    //HLT::ErrorCode stat = attachFeature(outputTE, trigBphysColl, Key, "TrigDiMuon");
    HLT::ErrorCode stat = recordAndAttachFeature(outputTE, trigBphysColl.release(), Key, "TrigDiMuon" );
    if (stat!=HLT::OK)
    {
      m_hist_error.push_back(TrigDiMuonErr::bPhysCollFails);
      msg() << MSG::ERROR << "Record into StoreGate failed" << endreq;
      return HLT::SG_ERROR;
    }
    outputTE->setActiveState(true);
  }

  if (trigVertexColl->size() > 0)
  {

    if (msgLvl() <= MSG::DEBUG)
      msg()  << MSG::DEBUG << " Store Vertex Collection size: " << trigVertexColl->size() << endreq;

    std::string Key = "";

    HLT::ErrorCode stat = recordAndAttachFeature(outputTE, trigVertexColl.release(), Key, "TrigDiMuon" );
        //stat = attachFeature(outputTE, trigVertexColl, Key, "TrigDiMuon");
        //if (sc != HLT::OK)
    if (stat != HLT::OK)
    {
      m_hist_error.push_back(TrigDiMuonErr::vtxCollFails);
      msg()  << MSG::WARNING << "Failed to store Vertex Collection" << endreq;
      return HLT::ERROR;
    }
    outputTE->setActiveState(true);
  }

  if (doTiming())
  {
    m_timerTotal->stop();
    m_timerTotal1->stop();
    m_timerAllFun->stop();
  }

  return HLT::OK;
}

HLT::ErrorCode TrigDiMuonFast::processTrackColl(const TrigInDetTrackCollection * theTrackColl,
    TrigL2BphysContainer * trigBphysColl,
    TrigVertexCollection * trigVertexColl,
    const LVL1::RecMuonRoI* m_pMuonRoI,
    const HLT::TriggerElement* inputTE,
    HLT::TriggerElement* outputTE)
{

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " (*theTrackColl)->size() = " << theTrackColl->size() << endreq;

  //clean up

  int jpsi_num = 0;

  m_pos_muon_barrel_mdt_hits.clear();
  m_neg_muon_barrel_mdt_hits.clear();
  m_pos_muon_cap_mdt_hits.clear();
  m_neg_muon_cap_mdt_hits.clear();
  m_pos_muon_tgc_hits.clear();
  m_pos_muon_rpc_hits.clear();
  m_neg_muon_tgc_hits.clear();
  m_neg_muon_rpc_hits.clear();
  m_pos_muon_eta.clear();
  m_pos_muon_phi.clear();
  m_pos_muon_pt.clear();
  m_neg_muon_eta.clear();
  m_neg_muon_phi.clear();
  m_neg_muon_pt.clear();

  m_j_psi_mass.clear();
  m_j_psi_mass_wide.clear();
  m_j_psi_eta.clear();
  m_j_psi_phi.clear();
  m_j_psi_pt.clear();
  m_j_psi_chi2.clear();
  m_j_psi_lifetime.clear();

  m_roiEndcap_dEta.clear();
  m_roiEndcap_dPhi.clear();
  m_roiEndcap_dR.clear();
  m_roiBarrel_dEta.clear();
  m_roiBarrel_dPhi.clear();
  m_roiBarrel_dR.clear();

  m_roiEndcap_ne_dEta.clear();
  m_roiEndcap_ne_dPhi.clear();
  m_roiEndcap_ne_dR.clear();
  m_roiBarrel_ne_dEta.clear();
  m_roiBarrel_ne_dPhi.clear();
  m_roiBarrel_ne_dR.clear();

  m_roiNoHits_pt.clear();
  m_roiNoHits_charge.clear();
  m_roiNoHits_eta.clear();
  m_roiNoHits_phi.clear();

  m_tap_mass_trTr.clear();
  m_tap_mass_trTr_os.clear();
  m_tap_mass_trRoI.clear();
  m_tap_mass_muRoI.clear();
  m_tap_mass_muMuRoI.clear();

  m_cnt_accTrk = 0;
  m_cnt_trTr = 0;
  m_cnt_muMu = 0;
  m_cnt_muMuVtx = 0;
  m_cnt_muMuRoI = 0;
  m_cnt_muMuRoIVtx = 0;

  m_hit_Endcap_mdt_inner_dEta.clear();
  m_hit_Endcap_mdt_middle_dEta.clear();
  m_hit_Barrel_mdt_inner_dEta.clear();
  m_hit_Barrel_mdt_middle_dEta.clear();
  m_hit_rpc_middle_dEta.clear();
  m_hit_rpc_middle_dPhi.clear();
  m_hit_tgc_wire_inner_dEta.clear();
  m_hit_tgc_wire_middle_dEta.clear();
  m_hit_tgc_wire_inner_dPhi.clear();
  m_hit_tgc_wire_middle_dPhi.clear();
  m_hit_tgc_strip_inner_dPhi.clear();
  m_hit_tgc_strip_middle_dPhi.clear();

  m_vtx_mass.clear();
  m_vtx_chi2.clear();
  m_vtx_ndof.clear();
  m_vtx_x   .clear();
  m_vtx_y   .clear();
  m_vtx_z   .clear();


  //monitoring: number of all tracks
  m_cnt_allTrk = theTrackColl->size();

  // First of all initialize the intermediate collection
  // The second parameter can have the following values:
  //    UNKNOWN: hit association wasn't performed yet
  //    NO_HITS_ON_ROD: there were no hits in ROD to which track is pointing
  //    MUON_ID_FAILED: number of associated hits is below threshold => track is not muon
  //    MUON_ID_SUCCESSFUL: number of associated hits is above threshold => track is muon

  std::vector<std::pair<TrigInDetTrack*, AssociationResult> > myCollection;

  TrigInDetTrackCollection::const_iterator
      it = theTrackColl->begin(),
      e = theTrackColl->end();
  for (; it != e; ++it)
  {
    //check track quality
    if (!(*it)->param())
      continue;
    if ((*it)->chi2()==100000000.000)
      continue;
    if (std::isnan((*it)->param()->pT()))
      continue;

    //Select track with pt above treshold
    if (fabs((*it)->param()->pT()) < m_PtMin)
      continue;

    //fill collection
    myCollection.push_back(std::pair<TrigInDetTrack*, AssociationResult>(*it, UNKNOWN));
  }

  m_cnt_accTrk = myCollection.size();

  // sort the collection according to Pt
  //std::sort(myCollection.begin(), myCollection.end(), CompPtGreater);

  //*** Tag and probe analysis *********************************************
  if(m_doTagAndProbe && m_retrieveRoI)
  {
    // Loop over all track pair combinations, but no decision is taken
    // -- just for monitoring and calibration purpose
    // If TagAndProbe is enabled, hit association will be
    // performed and results will be stored in myCollection,
    // so it won't need to be repeated in the next step.

    tagAndProbe(myCollection, m_pMuonRoI, inputTE, outputTE);
  }

  //************************************************************************

  //begin the loop over first track
  int nIdTracks=0;
  for (int i_tr1 = 0; i_tr1 < (int)myCollection.size(); ++i_tr1)
  {
    TrigInDetTrack * tr1 = myCollection[i_tr1].first;
    AssociationResult associated_tr1 = myCollection[i_tr1].second;

    double pt_idtr1 = tr1->param()->pT();
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " pt1 = " << pt_idtr1 << endreq;

    nIdTracks++;

    //begin the loop over the second track
    bool skip = false;
    for (int i_tr2 = i_tr1 + 1; i_tr2 < (int)myCollection.size() && !skip; ++i_tr2)
    {

      TrigInDetTrack * tr2 = myCollection[i_tr2].first;
      AssociationResult associated_tr2 = myCollection[i_tr2].second;

      double pt_idtr2 = tr2->param()->pT();
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << " pt2 = " << pt_idtr2 << endreq;

      m_hist_cand_lastStage.push_back(TrigDiMuonAcc::cand_gotTrkPair); //last stage of the algorithm executed

      if(m_hist_lastStage.back() < TrigDiMuonAcc::gotTrkPair)   //last stage of the algorithm executed
        m_hist_lastStage.push_back(TrigDiMuonAcc::gotTrkPair);

      //opposite charge check
      if (m_doOppChargeCheck && pt_idtr1*pt_idtr2 > 0)
      {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Pair rejected by opposite charge check" << endreq;

        continue;
      }

      m_hist_cand_lastStage.push_back(TrigDiMuonAcc::cand_gotOSTrkPair); //last stage of the algorithm executed

      if(m_hist_lastStage.back() < TrigDiMuonAcc::gotOSTrkPair)   //last stage of the algorithm executed
        m_hist_lastStage.push_back(TrigDiMuonAcc::gotOSTrkPair);

      if(!m_doOppChargeCheck && msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Opposite charge check disabled" << endreq;

      // calculate invariant mass
      // global variable j_psi_mon_vector used for monitoring is filled in this function
      double mass = invariant_mass(   fabs(pt_idtr1),
                                      tr1->param()->phi0(),
                                      tr1->param()->eta(),
                                      fabs(pt_idtr2),
                                      tr2->param()->phi0(),
                                      tr2->param()->eta());

      //cut on invariant mass
      if (mass < m_JpsiMassMin) continue;

      m_hist_cand_lastStage.push_back(TrigDiMuonAcc::cand_passInvMass); //last stage of the algorithm executed

      if(m_hist_lastStage.back() < TrigDiMuonAcc::passInvMass)   //last stage of the algorithm executed
        m_hist_lastStage.push_back(TrigDiMuonAcc::passInvMass);

      //monitoring
      ++m_cnt_trTr;

      //debug output
      if (msgLvl() <= MSG::DEBUG)
      {
        msg() << MSG::DEBUG << " JpsiMassMin =  " << m_JpsiMassMin << endreq;
        msg() << MSG::DEBUG << " mass =  " << mass << endreq;
        msg() << MSG::DEBUG << " first track of the pair from LIST "
            << " pt1 = "
            <<   pt_idtr1
            << " eta1 = "
            <<   tr1->param()->eta()
            << " phi1 = "
            <<   tr1->param()->phi0()
            << endreq;

        msg() << MSG::DEBUG << " second track of the pair from LIST "
            << " pt2 = "
            <<   pt_idtr2
            << " eta2 = "
            <<   tr2->param()->eta()
            << " phi2 = "
            <<   tr2->param()->phi0()
            << endreq;
      }

      // Hit association: find hits on track. Will be called only once per track
      //track 1: ************************************************************************
      if(associated_tr1 == UNKNOWN)
      {
        associated_tr1 = processTrack(tr1, i_tr1, 1, m_pMuonRoI, inputTE, outputTE);
        myCollection[i_tr1].second = associated_tr1;
      }

      //decide whether accept/reject track:
      //there are 2 possible cases: * if m_fill_esd=true then all the muon candidates that have some hits in ROD are kept
      //                            * otherwise number of hits on track is checked
      if(msgLvl() <= MSG::DEBUG && associated_tr1 == NO_HITS_ON_ROD)
        msg() << MSG::DEBUG << " First muon: no hits were found in the ROD" << i_tr1 << endreq;

      if ( (m_fill_esd && associated_tr1 == NO_HITS_ON_ROD) ||
           (!m_fill_esd && associated_tr1!=MUON_ID_SUCCESSFUL))
      {
        //skipping track
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Hit association failed, skipping candidate" << i_tr1 << endreq;

        skip = true;
        continue;
      }

      //track 2: ****************************************************************************
      if(associated_tr2 == UNKNOWN)
      {
        associated_tr2 = processTrack(tr2, i_tr2, 2, m_pMuonRoI, inputTE, outputTE);
        myCollection[i_tr2].second = associated_tr2;
      }

      //decide whether accept/reject track:
      //there are 2 possible cases: * if m_fill_esd=true then all the muon candidates that have some hits in ROD are kept
      //                            * otherwise number of hits on track is checked
      if(msgLvl() <= MSG::DEBUG && associated_tr2 == NO_HITS_ON_ROD)
        msg() << MSG::DEBUG << " Second muon:  no hits were found in the ROD" << i_tr2 << endreq;

      if ( (m_fill_esd && associated_tr2 == NO_HITS_ON_ROD) ||
           (!m_fill_esd && associated_tr2!=MUON_ID_SUCCESSFUL))
      {
        //skipping track
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Hit association failed, skipping candidate" << i_tr2 << endreq;

        //track will be removed so it can't be used again
        myCollection.erase(myCollection.begin() + i_tr2);
        --i_tr2;
        continue;
      }
      //*************************************************************************************

      m_hist_cand_lastStage.push_back(TrigDiMuonAcc::cand_passMuonId); //last stage of the algorithm executed

      if(m_hist_lastStage.back() < TrigDiMuonAcc::passMuonId)   //last stage of the algorithm executed
        m_hist_lastStage.push_back(TrigDiMuonAcc::passMuonId);

      // RoI association
      bool roiAssociated_tr1 = true;
      bool roiAssociated_tr2 = true;

      //monitoring
      if(associated_tr1 == MUON_ID_SUCCESSFUL && associated_tr2 == MUON_ID_SUCCESSFUL)
        ++m_cnt_muMu;

      if(m_retrieveRoI) {
        bool roiAssociated_tr1 = roiAssociation(tr1, m_pMuonRoI);
        bool roiAssociated_tr2 = roiAssociation(tr2, m_pMuonRoI);

        if(associated_tr1 == MUON_ID_SUCCESSFUL && associated_tr2 == MUON_ID_SUCCESSFUL &&
          (roiAssociated_tr1 || roiAssociated_tr2))
          ++m_cnt_muMuRoI;
      }

      // decision based on RoI matching *****************************************************
      if(m_doRoiCheck && m_retrieveRoI && !roiAssociated_tr1 && !roiAssociated_tr2)
      {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "RoI association failed, skipping candidate" << endreq;

        skip = true;
        continue;
      }

      m_hist_cand_lastStage.push_back(TrigDiMuonAcc::cand_passRoIAssoc); //last stage of the algorithm executed

      if(m_hist_lastStage.back() < TrigDiMuonAcc::passRoIAssoc)   //last stage of the algorithm executed
        m_hist_lastStage.push_back(TrigDiMuonAcc::passRoIAssoc);

      //*************************************************************************************

      if (msgLvl() <= MSG::DEBUG)
      {
        msg() << MSG::DEBUG << " mass of the FINAL pair = " << mass <<  endreq;
        msg() << MSG::DEBUG << " first track FINAL "
            << " pt1 = "
            <<   tr1->param()->pT()
            << " eta1 = "
            <<   tr1->param()->eta()
            << " phi1 = "
            <<   tr1->param()->phi0()
            << endreq;

        msg() << MSG::DEBUG << " second track FINAL "
            << " pt2 = "
            <<   tr2->param()->pT()
            << " eta2 = "
            <<   tr2->param()->eta()
            << " phi2 = "
            <<   tr2->param()->phi0()
            << endreq;
      }

      /// Create BPhys object
      int roiWord = 0;
      if(m_retrieveRoI && m_pMuonRoI!=NULL) roiWord = m_pMuonRoI->roiWord();
      TrigL2Bphys* trigPartBmumu = new TrigL2Bphys(roiWord, 0., 0., TrigL2Bphys::JPSIMUMU, mass);

      // Need to find index of tracks in the collection:
      TrigInDetTrackCollection::const_iterator it = theTrackColl->begin(),
      e = theTrackColl->end();
      int itr1=-99;
      int itr2=-99;
      for (int itrk=0; it != e; ++it,++itrk)
      {
        if (tr1 == (*it)) itr1=itrk;
        if (tr2 == (*it)) itr2=itrk;
      }
      if (itr1==-99 || itr2==-99) {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Something is worng, can't find tracks in original container" << endreq;
      }

      ElementLink<TrigInDetTrackCollection> track1EL(*theTrackColl,itr1);
      ElementLink<TrigInDetTrackCollection> track2EL(*theTrackColl,itr2);

      trigPartBmumu->addTrack(track1EL);
      trigPartBmumu->addTrack(track2EL);

      /// Vertexing ******
      //new L2 vertexing

      TrigVertex* p_mumuV = NULL;

      if(m_doVertexing)
      {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Performing vertex fit" << endreq;

        StatusCode sc;
        TrigL2Vertex* pL2V     = new TrigL2Vertex();

        //add track to the vertexer
        sc = m_vertexingTool->addTrack( tr1, pL2V, Trk::muon );
        if ( sc.isFailure() )
        {
          if ( msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "Failed to add track 1 to vertexingTool pT, chi2 " << tr1->param()->pT() << " " << tr1->chi2() << endreq;

          //error histogram
          m_hist_error.push_back(TrigDiMuonErr::addTrackFails);
        }
        else
        {

          sc = m_vertexingTool->addTrack( tr2, pL2V, Trk::muon );
          if ( sc.isFailure() )
          {
            if ( msgLvl() <= MSG::DEBUG)
              msg() << MSG::DEBUG << "Failed to add track 2 to vertexingTool pT, chi2 " << tr2->param()->pT() << " " << tr2->chi2() << endreq;

            //error histogram
            m_hist_error.push_back(TrigDiMuonErr::addTrackFails);
          }
          else
          {

            // DO THE VERTEX-FIT
            sc = m_L2vertFitter->fit(pL2V);
            if ( sc.isFailure() )
            {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endreq;

              //debug histogram
              m_hist_debug.push_back(TrigDiMuonDebug::vtxFailed);

            }
            else
            {

              sc = m_vertexingTool->calculateInvariantMass(pL2V);

              if ( sc.isFailure() )
              {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Can't calculate invariant mass" << endreq;

                //error histogram
                m_hist_error.push_back(TrigDiMuonErr::calcMassFails);

              }
              else
              {
                //create TrigVertex
                p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
              }
            }
          }
        }

        //delete pL2V, we don't need it any longer
        delete pL2V;

        //check the result
        if (p_mumuV == NULL)
        {
          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " vertex fit failed  " << endreq;

          //we want to keep the candidate even if vertexing failed for monitoring purpose
          //chi2 will be set to a large value so these candidates get excluded in the Hypo
          trigPartBmumu->fitchi2(100000);

        }
        else
        {

          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " vertex fit succeeded : chi2 = " << p_mumuV->chi2() << endreq;

          //store vertex fit results
          trigPartBmumu->fitmass(p_mumuV->mass());
          trigPartBmumu->fitchi2(p_mumuV->chi2());
          trigPartBmumu->fitndof(p_mumuV->ndof());
          trigPartBmumu->fitx(p_mumuV->x());
          trigPartBmumu->fity(p_mumuV->y());
          trigPartBmumu->fitz(p_mumuV->z());

          //save vertex
          trigVertexColl->push_back(p_mumuV);

          //monitoring
          //NOTE: if m_doRoiCheck is true those two histograms will be identical
          if(associated_tr1 == MUON_ID_SUCCESSFUL && associated_tr2 == MUON_ID_SUCCESSFUL)
            ++m_cnt_muMuVtx;

          if(m_retrieveRoI && associated_tr1 == MUON_ID_SUCCESSFUL && associated_tr2 == MUON_ID_SUCCESSFUL &&
             (roiAssociated_tr1 || roiAssociated_tr2))
            ++m_cnt_muMuRoIVtx;

          //monitoring
          m_vtx_mass.push_back(p_mumuV->mass());
          m_vtx_chi2.push_back(p_mumuV->chi2());
          m_vtx_ndof.push_back(p_mumuV->ndof());
          m_vtx_x   .push_back(p_mumuV->x()   );
          m_vtx_y   .push_back(p_mumuV->y()   );
          m_vtx_z   .push_back(p_mumuV->z()   );

        }

      }
      else
      {

        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Vertex fit NOT required" << endreq;
      }

      m_hist_cand_lastStage.push_back(TrigDiMuonAcc::cand_passVtx); //last stage of the algorithm executed

      if(m_hist_lastStage.back() < TrigDiMuonAcc::passVtx)   //last stage of the algorithm executed
        m_hist_lastStage.push_back(TrigDiMuonAcc::passVtx);

      // save the TrigBphysParticle ********************************************

      //debug
      if (msgLvl() <= MSG::DEBUG)
      {

        int32_t w1 = tr1->HitPattern();
        int32_t w2 = tr2->HitPattern();
        Encoding* e1 = (Encoding*)&w1;
        Encoding* e2 = (Encoding*)&w2;

        if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "DAN: candidate " << jpsi_num << " track 0; " <<
              "pt: " << tr1->param()->pT() << ", " <<
              "word: " << w1 << ", " <<
              "mdt: " << e1->mdt << ", " <<
              "rpc: " << e1->rpc << ", " <<
              "tgc: " << e1->tgc << ", " << endreq;
          msg() << MSG::DEBUG << "DAN: candidate " << jpsi_num << " track 1; " <<
              "pt: " << tr2->param()->pT() << ", " <<
              "word: " << w2 << ", " <<
              "mdt: " << e2->mdt  << ", " <<
              "rpc: " << e2->rpc  << ", " <<
              "tgc: " << e2->tgc  << ", " << endreq;
        }
      }

      ++jpsi_num;

      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Saving TrigBphysParticle" << endreq;

      trigBphysColl->push_back(trigPartBmumu);

      // Monitoring Jpsi //////////

      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << " mass for monitoring = " << mass << endreq;

      // NOTE J/Psi basics taken from the function "invariant_mass"
      m_j_psi_mass.push_back(mass);
      m_j_psi_mass_wide.push_back(mass);
      m_j_psi_eta.push_back(j_psi_mon_vector.eta());
      m_j_psi_phi.push_back(j_psi_mon_vector.phi());
      m_j_psi_pt.push_back(j_psi_mon_vector.vect().perp());

      if (p_mumuV == NULL)
      {  // J/Psi chi2 of Vertex fit
        m_j_psi_chi2.push_back(100000.);
        m_j_psi_lifetime.push_back(-1.);
      }
      else
      {
        m_j_psi_chi2.push_back(p_mumuV->chi2());
        // FIXME: currently the distance is taken from the origin (0,0,0) not from interaction point
        m_j_psi_lifetime.push_back(p_mumuV->position().distance());
      }
      // end of Monitoring Jpsi ///
    }
  }

  m_j_psi_per_roi = jpsi_num;

  //monitoring: merge m_hist_lastStage and m_hist_cand_lastStage vectors
  m_hist_lastStage.insert(m_hist_lastStage.end(), m_hist_cand_lastStage.begin(), m_hist_cand_lastStage.end());

  return HLT::OK;
}




HLT::ErrorCode TrigDiMuonFast::hltFinalize()
{
  msg() << MSG::INFO << "Finalizing TrigDiMuonFast" << endreq;
  return HLT::OK;
}

//bool TrigDiMuonFast::HitAssociation( const TrigInDetTrack* track,
//                                     Candidate & t,
//                                     int & n_mdt_hits,
//                                     int & n_rpc_eta_hits,
//                                     int & n_rpc_phi_hits,
//                                     int & n_tgc_eta_hits,
//                                     int & n_tgc_phi_hits)
bool TrigDiMuonFast::HitAssociation( TrigInDetTrack* track,
                                     Candidate & t,
                                     int & n_mdt_hits,
                                     int & n_rpc_eta_hits,
                                     int & n_rpc_phi_hits,
                                     int & n_tgc_eta_hits,
                                     int & n_tgc_phi_hits)
{
    //MsgStream log(messageService(), name());
    //StatusCode status = StatusCode::SUCCESS;

    //Candidate t;
  t.setParameters(fabs(track->param()->pT()), track->param()->eta(), track->param()->phi0(), (int)(track->param()->pT()/fabs(track->param()->pT())));

  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << " HitAssociation function for track "
        << "  Pt     =  " << t.Pt()
        << "  Eta    =  " << t.Eta()
        << "  Phi    =  " << t.Phi()
        << "  Charge =  " << t.Charge() << endreq;
  }

    // Preload ROB data
    //unsigned int rob_id = 0;

	//m_tgc_lut_digits.clear();
  m_rpc_raw_data_list.clear();
  m_mdt_tubes.clear();

  if (fabs(t.Eta())< 1.1)
    t.doRpc(true);
  if (fabs(t.Eta()) > 0.9)
    t.doTgc(true);


  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " TGC = " << t.Tgc() << endreq;

  if (t.Tgc())
  {
    if (doTiming())
      m_timerTotalTgc->start();
    makeTGC(t);
    if (doTiming())
      m_timerTotalTgc->stop();
  }
  else
    m_tgc_lut_digits.clear();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " RPC = " << t.Rpc() << endreq;


  if (t.Rpc())
  {
    if (doTiming())
      m_timerTotalRpc->start();
    makeRPC(t);
    if (doTiming())
      m_timerTotalRpc->stop();
  }

  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << "   Number of TGC eta hits   =   " << t.NTgcEtaHits() <<
        "   Number of TGC phi hits   =   " << t.NTgcPhiHits() <<
        "   Number of RPC eta hits   =   " << t.NRpcEtaHits() <<
        "   Number of RPC phi hits   =   " << t.NRpcPhiHits() << endreq;
  }
    //if ((t.NTgcEtaHits() + t.NTgcPhiHits()) <= 2 && (t.NRpcEtaHits()+t.NRpcPhiHits()) <= 2)
    //{
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " doMDT " << endreq;

  makeMDT(t);
    //}
  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << "   Number of MDT hits   =   " << t.NMdtHits() <<
        "   Number of TGC eta hits   =   " << t.NTgcEtaHits() <<
        "   Number of TGC phi hits   =   " << t.NTgcPhiHits() <<
        "   Number of RPC eta hits   =   " << t.NRpcEtaHits() <<
        "   Number of RPC phi hits   =   " << t.NRpcPhiHits() << endreq;
  }

  n_mdt_hits = t.NMdtHits();
  n_rpc_eta_hits = t.NRpcEtaHits();
  n_rpc_phi_hits = t.NRpcPhiHits();
  n_tgc_eta_hits = t.NTgcEtaHits();
  n_tgc_phi_hits = t.NTgcPhiHits();

    //final selection
  if ((t.NMdtHits() + t.NRpcEtaHits() + t.NRpcPhiHits()) >= m_NHits ||
       (t.NMdtHits() + t.NTgcEtaHits() + t.NTgcPhiHits()) >= m_NHits)
    return true;

  return false;
}

double TrigDiMuonFast::invariant_mass(double m_pt1, double m_phi1, double m_eta1,
                                      double m_pt2, double m_phi2, double m_eta2 )
{
  double m_cot1=1/tan(2*atan(exp(-m_eta1)));
  double m_cot2=1/tan(2*atan(exp(-m_eta2)));
  double mass_muon = 105.658357;

  double px1=cos(m_phi1)*m_pt1;
  double py1=sin(m_phi1)*m_pt1;
  double pz1=m_cot1*m_pt1;
  double E1= sqrt(px1*px1+py1*py1+pz1*pz1+mass_muon*mass_muon);

  double px2=cos(m_phi2)*m_pt2;
  double py2=sin(m_phi2)*m_pt2;
  double pz2=m_cot2*m_pt2;
  double E2= sqrt(px2*px2+py2*py2+pz2*pz2+mass_muon*mass_muon);

  CLHEP::HepLorentzVector vector1,vector2;
  vector1 = CLHEP::HepLorentzVector(px1,py1,pz1,E1);
  vector2 = CLHEP::HepLorentzVector(px2,py2,pz2,E2);

  double invariant_mass=vector1.invariantMass(vector2);
    //Monitoring j/psi vector;
  j_psi_mon_vector=vector1+vector2;

  return invariant_mass;
}

struct tgcDigitData
{
  int iDigit;
  double x, y, z, r, eta, phi;
  char sw, sel;
  std::string stationName;
};

void TrigDiMuonFast::makeMDT(Candidate& t)
{
  // retrieve the geometry on every event we are sure its initialization is
  // tuned with the database updates everytime
  m_pMdtGeometry = m_MDTcablingSvc->mdtGeometry();

  double eta = t.EtaExtrapolatedInnerEndCap();
  double phi = t.PhiExtrapolatedInnerEndCap();
  barrel_mdt_mon_flag=0;// this is for monitoring purpose
  if (fabs(t.Eta())<1.)
  {
    eta = t.EtaExtrapolatedInnerBarrel();
    phi = t.PhiExtrapolatedInnerBarrel();
    barrel_mdt_mon_flag=1; // this is for monitoring purpose
  }
  if (phi<0) phi = phi+2.*M_PI;

  double etaMin_mdt = eta - (m_MdtEtaFactor*44.638)/pow(t.Pt(),0.9109);
  double etaMax_mdt = eta + (m_MdtEtaFactor*44.638)/pow(t.Pt(),0.9109);
  double phiMin_mdt = phi - 0.1;
  double phiMax_mdt = phi + 0.1;

  if (phiMin_mdt < 0.) phiMin_mdt = 0.;
  if (phiMax_mdt > 2*M_PI) phiMax_mdt = 2*M_PI;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " etaMin_mdt = " << etaMin_mdt
        << " etaMax_mdt = " << etaMax_mdt
        << " phiMin_mdt = " << phiMin_mdt
        << " phiMax_mdt = " << phiMax_mdt
        << endreq;

  std::vector<uint32_t> robIds = m_pMdtGeometry->getRobId(etaMin_mdt, etaMax_mdt, phiMin_mdt, phiMax_mdt);
  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << " Muon MDT Cabling: Number of ROBIds = " << robIds.size() << endreq;
    msg() << MSG::DEBUG << " Muon MDT Cabling: List of ROBIds   = "  << endreq;
    for (std::vector<uint32_t>::const_iterator i=robIds.begin();i != robIds.end(); ++i)
    {
      msg() << MSG::DEBUG << std::hex << *i << std::dec << endreq;
    }
  }

  //accessing ROB data
  m_pROBDataProvider->addROBData(robIds);
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobs;
  vecRobs.clear();
  m_pROBDataProvider->getROBData(robIds,vecRobs);

  int n_mdt_dig=0;
  std::vector<IdentifierHash> mdt_hash_ids;

  double eta_mdt = 0.5*(etaMin_mdt+etaMax_mdt);
  double phi_mdt = HLT::phiMean(phiMin_mdt, phiMax_mdt);

  TrigRoiDescriptor _roi( eta_mdt, etaMin_mdt, etaMax_mdt, phi_mdt, phiMin_mdt, phiMax_mdt );

  m_pRegionSelector->DetHashIDList( MDT, _roi, mdt_hash_ids );

    //convert ROB data
  if (m_mdt_converter->convert(vecRobs,mdt_hash_ids).isFailure())
  {
    msg() << MSG::ERROR
        << "Failed to convert MDT CSM hash Ids: " << endreq;
    for (unsigned int i = 0; i < mdt_hash_ids.size(); i++)
    {
      m_hist_error.push_back(TrigDiMuonErr::convMdtFails);
      msg() << MSG::ERROR << mdt_hash_ids[i] << endreq;
    }
  }

  std::vector<IdentifierHash>::iterator
      mdthid = mdt_hash_ids.begin(),
      mdthid_end = mdt_hash_ids.end();
  for (;mdthid != mdthid_end; ++mdthid)
  {

    MdtCsmContainer::const_iterator csm = pMdtContainer->indexFind(*mdthid);
    if (csm != pMdtContainer->end() && !(*csm)->empty())
    {
      if (msgLvl() <= MSG::DEBUG)
        msg() <<  MSG::DEBUG <<  "Number of digit in Collection: " << (*csm)->size() << endreq;
      unsigned short int SubsystemId = (*csm)->SubDetId();
      unsigned short int MrodId = (*csm)->MrodId();
      unsigned short int LinkId = (*csm)->CsmId();
      CSMid csmId = m_pMdtGeometry->getCsm(SubsystemId,MrodId,LinkId);
      if (csmId)
      {
        const Chamber* cha = csmId.get_chamber();
        int StationName = csmId.stationName();
                //inner distance: BIL-0,BIS-1,BIR-7,BIM-52,EIC-12,EIL-13,FIL-35,FIS-36,EIS-49
        if (StationName != 0 &&
            StationName != 1 &&
            StationName != 7 &&
            StationName != 52 &&
            StationName != 35 &&
            StationName != 36 &&
            StationName != 12 &&
            StationName != 13 &&
            StationName != 49)
          continue;
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << " == StationName == "<< StationName << endreq;

        MdtCsm::const_iterator amt = (*csm)->begin();
        for (; amt != (*csm)->end(); ++amt )
                //while (amt != (*csm)->end())
        {
          if ((*amt)->leading())
          {
            unsigned short int TdcId = (*amt)->tdcId();
            unsigned short int ChannelId = (*amt)->channelId();
            ( (*amt)->coarse() << 5);

            TDCgeo* tdc = cha->TDC[TdcId];
            if (!tdc)
              continue;
                        // error because the corresponding TDC is nt found into the cabling map
                        // display some error messages
                        // do the proper action

            double Z = tdc->RZ[ChannelId][1];    // Z of the hit tube
            double R = tdc->RZ[ChannelId][0];    // R of hit tube

            if(R==0)
              continue;

            TrigDiMuon::mdtTube mdttube;
            mdttube.mdt_TdcId = TdcId;
            mdttube.mdt_ChannelId = ChannelId;
            mdttube.mdt_tube_z = (float)Z;
            mdttube.mdt_tube_r = (float)R;
            mdttube.associated = 0.;

            double eta_hit = -log(tan((atan2(R,Z))/2.));

            //monitoring MDT, inner station
            if(barrel_mdt_mon_flag)
              m_hit_Barrel_mdt_inner_dEta.push_back(eta_hit - eta);
            else
              m_hit_Endcap_mdt_inner_dEta.push_back(eta_hit - eta);

            if( m_extrapolator->matchMdtInner(R,Z,t) )
            {
              n_mdt_dig++;
              mdttube.associated = 1.;
            }
            m_mdt_tubes.push_back(mdttube);
            if (msgLvl() <= MSG::DEBUG)
            {
              msg() << MSG::DEBUG << " eta_hit         = " << eta_hit
                  << " eta of Mdt hits = " << eta
                  << " delta eta       = " << fabs(eta_hit - eta)
//                   << " window_mdt_eta  = " << window_mdt_eta
                  << " n_mdt_dig  = "  << n_mdt_dig << endreq;
            }
          }
        }
      }
    }
  }
  if (true) //(n_mdt_dig == 0)
  {
    double eta_middle = t.EtaExtrapolatedMiddleEndCap();

    double etaMin_mdt_middle = eta_middle - m_MdtEtaFactor*131.57/pow(t.Pt(),0.978);
    double etaMax_mdt_middle = eta_middle + m_MdtEtaFactor*131.57/pow(t.Pt(),0.978);;

    double phi_middle = t.PhiExtrapolatedMiddleEndCap();
    if (fabs(t.Eta())<1.)
    {
      eta_middle = t.EtaExtrapolatedMiddleBarrel();
      etaMin_mdt_middle = eta_middle - m_MdtEtaFactor*42.654/pow(t.Pt(),0.8662);
      etaMax_mdt_middle = eta_middle + m_MdtEtaFactor*42.654/pow(t.Pt(),0.8662);
      phi_middle = t.PhiExtrapolatedMiddleBarrel();
    }
    if (phi_middle<0) phi_middle = phi_middle+2.*M_PI;

    double phiMin_mdt_middle = phi_middle - 0.2;
    double phiMax_mdt_middle = phi_middle + 0.2;

    if (phiMin_mdt < 0.) phiMin_mdt = 0.;
    if (phiMax_mdt > 2*M_PI) phiMax_mdt = 2*M_PI;

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " etaMin_mdt_middle = " << etaMin_mdt_middle
          << " etaMax_mdt_middle = " << etaMax_mdt_middle
          << " phiMin_mdt_middle = " << phiMin_mdt_middle
          << " phiMax_mdt_middle = " << phiMax_mdt_middle
          << endreq;

    std::vector<IdentifierHash> mdt_hash_ids_middle;

    double eta_mdt_middle = 0.5*(etaMin_mdt_middle+etaMax_mdt_middle);
    double phi_mdt_middle = HLT::phiMean(phiMin_mdt_middle, phiMax_mdt_middle);
    
    TrigRoiDescriptor _roi( eta_mdt_middle, etaMin_mdt_middle, etaMax_mdt_middle, 
			    phi_mdt_middle, phiMin_mdt_middle, phiMax_mdt_middle );
    
    m_pRegionSelector->DetHashIDList( MDT, _roi, mdt_hash_ids_middle ); 
                                      

        //convert ROB data
    if (m_mdt_converter->convert(vecRobs,mdt_hash_ids_middle).isFailure())
    {
      m_hist_error.push_back(TrigDiMuonErr::convMdtFails);
      msg() << MSG::ERROR
          << "Failed to convert MDT CSM hash Ids: " << endreq;
      for (unsigned int i = 0; i < mdt_hash_ids_middle.size(); i++)
      {
        msg() << MSG::ERROR << mdt_hash_ids_middle[i] << endreq;
      }
    }

    std::vector<IdentifierHash>::iterator
        mdthid_m = mdt_hash_ids_middle.begin(),
    mdthid_m_end = mdt_hash_ids_middle.end();


    for (;mdthid_m != mdthid_m_end; ++mdthid_m)
    {
      MdtCsmContainer::const_iterator csm_m = pMdtContainer->indexFind(*mdthid_m);
      if (csm_m != pMdtContainer->end() && !(*csm_m)->empty())
      {
        if (msgLvl() <= MSG::DEBUG)
          msg() <<  MSG::DEBUG <<  "Number of digit in Collection: " << (*csm_m)->size() << endreq;
        unsigned short int SubsystemId_m = (*csm_m)->SubDetId();
        unsigned short int MrodId_m = (*csm_m)->MrodId();
        unsigned short int LinkId_m = (*csm_m)->CsmId();
        CSMid csmId_m = m_pMdtGeometry->getCsm(SubsystemId_m,MrodId_m,LinkId_m);
        if (csmId_m)
        {
          const Chamber* cha_m = csmId_m.get_chamber();
          int StationName_m = csmId_m.stationName();
                    //inner distance: BML-2,BMS-3,BMF-8,EMC-16,EML-17,EMS-18,FML-37,FMS-38
          if (StationName_m != 2 &&
              StationName_m != 3 &&
              StationName_m != 8 &&
              StationName_m != 16 &&
              StationName_m != 17 &&
              StationName_m != 18 &&
              StationName_m != 37 &&
              StationName_m != 38)
            continue;
          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " == StationName_m == "<< StationName_m << endreq;

          MdtCsm::const_iterator amt_m = (*csm_m)->begin();
          for(; amt_m != (*csm_m)->end(); ++amt_m )
                    //while (amt_m != (*csm_m)->end())
          {
            if ((*amt_m)->leading())
            {
              unsigned short int TdcId_m = (*amt_m)->tdcId();
              unsigned short int ChannelId_m = (*amt_m)->channelId();
              ( (*amt_m)->coarse() << 5);

              TDCgeo* tdc_m = cha_m->TDC[TdcId_m];
              if (!tdc_m)
                continue;

              double Z_m = tdc_m->RZ[ChannelId_m][1];    // Z of the hit tube
              double R_m = tdc_m->RZ[ChannelId_m][0];    // R of hit tube

              if(R_m==0)
                continue;

              TrigDiMuon::mdtTube mdttube;
              mdttube.mdt_TdcId = TdcId_m;
              mdttube.mdt_ChannelId = ChannelId_m;
              mdttube.mdt_tube_z = (float)Z_m;
              mdttube.mdt_tube_r = (float)R_m;
              mdttube.associated = 0.;

              double eta_hit_m = -log(tan((atan2(R_m,Z_m))/2.));

              //monitoring, MDT, middle station
              if(barrel_mdt_mon_flag)
                m_hit_Barrel_mdt_middle_dEta.push_back(eta_hit_m - eta_middle);
              else
                m_hit_Endcap_mdt_middle_dEta.push_back(eta_hit_m - eta_middle);

              //association cut
              if ( m_extrapolator->matchMdtMiddle(R_m,Z_m,t) )
              {
                n_mdt_dig++;
                mdttube.associated = 1.;
              }
              m_mdt_tubes.push_back(mdttube);

              if (msgLvl() <= MSG::DEBUG)
              {
                msg() << MSG::DEBUG << " eta_hit_m         = " << eta_hit_m
                    << " eta of Mdt hits middle = " << eta_middle
                    << " delta eta       = " << fabs(eta_hit_m - eta_middle)
//                     << " window_mdt_eta_m  = " << window_mdt_eta_m
                    << " n_mdt_dig  = "  << n_mdt_dig << endreq;
              }
            }
                        //amt_m++;
          }
        }
      }
    }
  }

  t.setNMdtHits(n_mdt_dig);

  return;
}

void TrigDiMuonFast::makeRPC(Candidate& t)
{
    //StatusCode status = StatusCode::SUCCESS;
    //MsgStream log(msgSvc(), name());

  m_pRpcGeometry=m_RPCgeometrySvc->rpcGeometry();

  RpcPadIdHash padHashFunction = *(pRpcCabling->padHashFunction());


  if (doTiming())
    m_timerRpcProp->start();

  m_extrapolator->propagationInnerBarrel(t);
  m_extrapolator->propagationMiddleBarrel(t);

  if (doTiming())
    m_timerRpcProp->stop();

  double min_eta = t.EtaExtrapolatedMiddleBarrel() - (m_RpcEtaFactor*42.65)/pow(t.Pt(),0.8662);
  double max_eta = t.EtaExtrapolatedMiddleBarrel() + (m_RpcEtaFactor*42.65)/pow(t.Pt(),0.8662);
  double min_phi = t.PhiExtrapolatedMiddleBarrel() - (m_RpcPhiFactor*241.76)/pow(t.Pt(),1.038);
  double max_phi = t.PhiExtrapolatedMiddleBarrel() + (m_RpcPhiFactor*241.76)/pow(t.Pt(),1.038);

  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << "  phi extrapolated RPC Middle  =  " << t.PhiExtrapolatedMiddleBarrel() <<
        "  eta extrapolated RPC Middle  =  " << t.EtaExtrapolatedMiddleBarrel() << endreq;
    msg() << MSG::DEBUG << " min_eta = " << min_eta << " max_eta = " << max_eta << " min_phi = " << min_phi << " max_phi = " << max_phi << endreq;
  }

    //std::vector<uint32_t> outputROBIDList_rpc;

  std::vector<uint32_t> outputROBIDList_rpc = m_RPCgeometrySvc->getRobId(min_eta, max_eta, min_phi, max_phi);
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " outputROBIDList_rpc.size() = " << outputROBIDList_rpc.size() << endreq;


  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobs;
  m_pROBDataProvider->addROBData(outputROBIDList_rpc);
  vecRobs.clear();
  m_pROBDataProvider->getROBData(outputROBIDList_rpc,vecRobs);

  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << " Number of RPC ROBIds = " << outputROBIDList_rpc.size() << endreq;
    for (std::vector<uint32_t>::const_iterator i=outputROBIDList_rpc.begin();i != outputROBIDList_rpc.end(); ++i)
    {
      msg() << MSG::DEBUG << std::hex << *i << std::dec << endreq;
    }
  }

  std::vector<uint32_t> UIntList_rpc = m_RPCgeometrySvc->getPadHash(min_eta, max_eta, min_phi, max_phi);
  std::vector<IdentifierHash> HashIDList_rpc(UIntList_rpc.size());
  std::copy(UIntList_rpc.begin(), UIntList_rpc.end(), HashIDList_rpc.begin());
  if (m_rpc_converter->convert(vecRobs,HashIDList_rpc).isFailure())
  {
    m_hist_error.push_back(TrigDiMuonErr::convRpcFails);
    if (!doTiming())
      msg() << MSG::ERROR
          << "Failed to convert PAD offline Id " << endreq;
  }

  for (int k = 0; k < (int) HashIDList_rpc.size(); ++k)
  {
    RPCrawData rpc_digits;
    IdentifierHash pad_id = HashIDList_rpc[k];

    // convert and retrieve PAD

    RpcPadContainer::const_iterator itPAD = m_pRpcPadContainer->indexFind(pad_id);
    if (itPAD==m_pRpcPadContainer->end())
    {
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Failed to retrieve PAD hash Id " << pad_id << endreq;
      continue;
    }
    const RpcPad* rpc_pad = *itPAD;
    if (rpc_pad != NULL)
    {
      unsigned long int sector = (long) rpc_pad->sector();
      unsigned short int SubSystemId = (sector<32) ? 0 :1;
      unsigned short int SectorId = sector%32;
      unsigned short int PadId = rpc_pad->onlineId();
      //if (msgLvl() <= MSG::DEBUG)
      //  log << MSG::DEBUG << " sector = " << sector <<" SubSystemId = " << SubSystemId << " SectorId = " << SectorId << " PadId = " << PadId << endreq;
            //for each Pad loop over CMs
      RpcPad::const_iterator
          itPad = rpc_pad->begin(),
          itPadEnd = rpc_pad->end();
      for (; itPad!=itPadEnd; ++itPad)
      {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "pad_size=" << (*itPad)->size() << endreq;
        const RpcCoinMatrix * cma = *itPad;
        unsigned long int cmaId = (long) cma->onlineId();
        unsigned short int CMAId = cmaId & 3;
        unsigned short int lh    = (cmaId >> 2) & 1;
        const CMAparameters* CMA = pRpcCabling->getRPCCabling()->give_CMA(SubSystemId,SectorId,PadId,CMAId);
        if( CMA!=0 )
        {
          std::list <unsigned int> CodeList;
          //for each CM loop over fired channels
          RpcCoinMatrix::const_iterator itCm = (*itPad)->begin();
          RpcCoinMatrix::const_iterator itCmEnd = (*itPad)->end();
          for (; itCm!=itCmEnd; ++itCm)
          {
            CodeList.clear();
            const RpcFiredChannel * fChannel = (*itCm);
            unsigned short int ijk = fChannel->ijk();
            unsigned short int channel = fChannel->channel();
            float time = fChannel->time();
            if (msgLvl() <= MSG::DEBUG)
              msg() << MSG::DEBUG << "ijk = " << ijk << " channel = " << channel << endreq;

            CMA->give_strip_code(sector,lh,ijk,channel,CodeList);
	    RPCrawDigit Digit(0,CodeList,time,m_pRpcGeometry);
            //const RPCdigit* digit =
            rpc_digits << Digit;
          }
        }
      }
      m_rpc_raw_data_list.push_back(rpc_digits);
      if (doTiming())
        m_timerRpcData->pause();

      if (doTiming())
        m_timerRpcPoints->start();
      prepareRpcPoints(t, rpc_digits);
      if (doTiming())
        m_timerRpcPoints->stop();
      if (doTiming())
        m_timerRpcData->resume();
    }
  }
  if (doTiming())
    m_timerRpcData->stop();

  return;

}

void TrigDiMuonFast::makeTGC(Candidate& t)
{

  if (doTiming())
    m_timerTgcProp->start();

  m_extrapolator->propagationInnerEndCap(t);
  m_extrapolator->propagationMiddleEndCap(t);

  if (doTiming())
    m_timerTgcProp->stop();

  double phi = t.PhiExtrapolatedMiddleEndCap();
  double eta = t.EtaExtrapolatedMiddleEndCap();
  if (phi < 0) phi += 2.0 * M_PI;

  Identifier tgc_rdo_id;
  tgc_rdo_id = m_pTgcLUTSvc->rdoID(eta, phi);
  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG  << " tgc_rdo_id = " << tgc_rdo_id << endreq;
    msg() << MSG::DEBUG << " TGC RoI processing works! " << endreq;
  }

    //int tgc_rdo_no = int((phi + FIFTEEN_DEG) / FORTY_FIVE_DEG) % 8;
    //Identifier tgc_rdo_id;
    //if (t.EtaExtrapolatedMiddleEndCap() < 0.0)
    //        tgc_rdo_no += 8;
    //tgc_rdo_id = tgc_rdo_no;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "tgc_rdo_id = " << tgc_rdo_id << endreq;

  if (doTiming())
    m_timerTgcLUT->start();

  if (m_doLutTgcRdo)
  {
    if (tgc_rdo_id != tgc_rdo_id_previous || m_tgc_lut_digits.empty())
    {
      m_tgc_lut_digits.clear();
            // preload TGC ROB data for the track
      unsigned int trigger_rob_id = m_pTgcLUTSvc->robNumber(eta, phi);
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << " Tgc ROBId = " << std::hex << trigger_rob_id << std::dec << endreq;
      std::vector<uint32_t> robIds;
      robIds.clear();
      robIds.insert(robIds.begin(), trigger_rob_id);
      m_pROBDataProvider->addROBData(robIds);


      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobs;

      m_pROBDataProvider->                                                  //
          getROBData(std::vector<uint32_t>(1,trigger_rob_id),vecRobs);

      IdentifierHash tgc_id = pTgcRdoContainer->idToHash(tgc_rdo_id.get_identifier32().get_compact());

      if (m_tgc_converter->convert(vecRobs).isFailure())
      {
        m_hist_error.push_back(TrigDiMuonErr::convTgcFails);
        msg() << MSG::ERROR
            << "Failed to convert TGC RDO Id "
            << tgc_rdo_id
            << endreq;
        return;
      }

      TgcRdoContainer::const_iterator itTGC =  pTgcRdoContainer->indexFind(tgc_id);
      if (itTGC==pTgcRdoContainer->end())
      {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Failed to retrieve TGC hash Id " << tgc_id << endreq;
        return;
      }

      tgc_rdo_id_previous = tgc_rdo_id;

      const TgcRdo* tgc_rdo = *itTGC;
            //TgcRdoContainer::KEY key = pTgcRdoContainer->key(tgc_rdo_id);
            //if (m_pStoreGate->contains<TgcRdo>(key))
            //{
            //StatusCode status = m_pStoreGate->retrieve(tgc_rdo,key);
            //if (status.isFailure() || tgc_rdo == NULL)
            //{
            //  if (msgLvl() <= MSG::DEBUG)
            //    msg() << MSG::DEBUG << " Retrieval of TgcRdo key failed " << endreq;
            //  return;
            //}

            // Loop over RawData, converter RawData to digit
            // retrieve/create digit collection, and insert digit into collection
      TgcRdo::const_iterator itD  = tgc_rdo->begin();
      TgcRdo::const_iterator itDe = tgc_rdo->end();

      for (; itD != itDe; ++itD)
      {
                // check Hit or Coincidence
        if ((*itD)->isCoincidence()) continue;
                // select current Hits
        if ((*itD)->bcTag() != 0 && (*itD)->bcTag() != 2) continue;

        const TGCgeometrySvc::Entry& e = m_pTgcLUTSvc->getEntry((*itD)->subDetectorId(), (*itD)->rodId(), (*itD)->sswId(), (*itD)->slbId(), (*itD)->bitpos());
        TrigDiMuon::tgcLUTDigit lutDigit;
        lutDigit.eta     = e.eta;
        lutDigit.phi     = e.phi;
        lutDigit.r       = e.r;
        lutDigit.z       = e.z;
        lutDigit.width   = e.width;
        lutDigit.sta     = e.sta;
        lutDigit.isStrip = e.isStrip;
        lutDigit.associated = false;
        m_tgc_lut_digits.push_back(lutDigit);
      }
            //}
    }
  }

  if (doTiming())
    m_timerTgcLUT->stop();

  if (doTiming())
    m_timerTgcPoints->start();

  prepareTgcPoints(t);

  if (doTiming())
    m_timerTgcPoints->stop();

  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << "  Number of TGC eta Hit  =  " << t.NTgcEtaHits() <<
        "  Number of TGC phi Hit  =  " << t.NTgcPhiHits() << endreq;
  }
  return;
}

void TrigDiMuonFast::prepareRpcPoints( Candidate& t, RPCrawData rpc_digits)
{

  int n_rpc_eta_dig = 0;
  int n_rpc_phi_dig = 0;
  const RPCrawData::DigitList etaDigits = rpc_digits.eta_digits();
  const RPCrawData::DigitList phiDigits = rpc_digits.phi_digits();

  RPCrawData::DigitList::const_iterator eta = etaDigits.begin();
    //float RpcZeta[3] = {0.,0.,0.};
    //while (eta!=etaDigits.end())
  for (; eta != etaDigits.end(); ++eta)
  {
        //float v_rpcEtaDigit[3];
    float eta_rpcEtaDigit;
    float phi_rpcEtaDigit;
        //(*eta).xyz(v_rpcEtaDigit);
    (*eta).eta(eta_rpcEtaDigit);
    (*eta).phi(phi_rpcEtaDigit);

        //int station_eta = (*eta).decoding().lvl1_station() - 1;

    double delta_eta = eta_rpcEtaDigit - t.EtaExtrapolatedMiddleBarrel();

    //monitoring RPC, middle station, dEta
    m_hit_rpc_middle_dEta.push_back(delta_eta);

    if ( m_extrapolator->matchRpcEtaPad(eta_rpcEtaDigit, t) )
      n_rpc_eta_dig++;

    if (msgLvl() <= MSG::DEBUG)
    {
      msg() << MSG::DEBUG << " -------------- RPC eta Digits ---------------" <<
          " eta Digit =        " << eta_rpcEtaDigit <<
          " eta extrapolated = " << t.EtaExtrapolatedMiddleBarrel() <<
          " delta_eta =        " << delta_eta <<
//           " window_rpc_eta =   " << window_rpc_eta <<
          " n_rpc_eta_dig =    " << n_rpc_eta_dig << endreq;
    }
        //++eta;
  }

  RPCrawData::DigitList::const_iterator phi = phiDigits.begin();
    //while (phi!=phiDigits.end())
  for (; phi != phiDigits.end(); ++phi)
  {
        //float v_rpcPhiDigit[3];
    float eta_rpcPhiDigit;
    float phi_rpcPhiDigit;
        //(*phi).xyz(v_rpcPhiDigit);
    (*phi).eta(eta_rpcPhiDigit);
    (*phi).phi(phi_rpcPhiDigit);

        //int station_phi = (*phi).decoding().lvl1_station() - 1;

    double delta_phi = phi_rpcPhiDigit-t.PhiExtrapolatedMiddleBarrel();
    if (delta_phi > 3.*M_PI/2.) delta_phi -= 2.*M_PI ;
    if (delta_phi < -3.*M_PI/2.) delta_phi += 2.*M_PI ;

    //monitoring RPC, middle station, dPhi
    m_hit_rpc_middle_dPhi.push_back(delta_phi);

    if ( m_extrapolator->matchRpcPhiPad(phi_rpcPhiDigit, t) )
      n_rpc_phi_dig++;

    if (msgLvl() <= MSG::DEBUG)
    {
      msg() << MSG::DEBUG << " -------------- RPC phi Digits ---------------" <<
          " phi Digit =        " << phi_rpcPhiDigit <<
          " phi extrapolated = " << t.PhiExtrapolatedMiddleBarrel() <<
          " delta_phi =        " << delta_phi <<
//           " window_rpc_phi =   " << window_rpc_phi <<
          " n_rpc_phi_dig =    " << n_rpc_phi_dig << endreq;
    }
        //++phi;
  }
  t.setNRpcEtaHits(n_rpc_eta_dig);
  t.setNRpcPhiHits(n_rpc_phi_dig);

  return;

}

void TrigDiMuonFast::prepareTgcPoints(Candidate& t)
{
    //MsgStream log(msgSvc(), name());
  int n_tgc_phi_hits = 0, n_tgc_eta_hits = 0;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " prepare TGC hits " << endreq;

  if (!doTiming())
  {
    if (msgLvl() <= MSG::DEBUG) {
      if (m_doLutTgcRdo)
        msg() << MSG::DEBUG << " m_tgc_lut_digits.size() = " << m_tgc_lut_digits.size() << endreq;
      else
        msg() << MSG::DEBUG << " m_TgcDigitVector.size() = " << m_TgcDigitVector.size() << endreq;
    }
  }
    // loop over TGC digits.
  if (m_doLutTgcRdo)
  {
    for (int iDigit = 0; iDigit < (int)m_tgc_lut_digits.size(); iDigit++)
    {
      TrigDiMuon::tgcLUTDigit& digit = m_tgc_lut_digits[iDigit];
      if (digit.isStrip)
      {
        //TGC middle station
        if (digit.sta < 3)
        {
          double phi_ext_middle = t.PhiExtrapolatedMiddleEndCap();
          if (phi_ext_middle<0) phi_ext_middle = phi_ext_middle+2.*M_PI;

          double phi_digit = digit.phi;
          if (phi_digit<0) phi_digit = phi_digit+2.*M_PI;

          //monitoring TGC, middle station, dPhi
          m_hit_tgc_strip_middle_dPhi.push_back(phi_digit - phi_ext_middle);

          if (m_extrapolator->matchMiddleTgcPhiHit(phi_digit, t) )
          {
            n_tgc_phi_hits++;
            digit.associated = true;
          }

          if (msgLvl() <= MSG::DEBUG)
          {
            msg() << MSG::DEBUG << "  Phi of TGC phi Hit         =  " << phi_digit <<
                "  Eta of TGC phi Hit         =  " << digit.eta <<
                "  Phi of extrapolated track  =  " << phi_ext_middle <<
                "  Delta phi                  =  " << fabs(phi_digit - phi_ext_middle) <<
//                 "  Delta phi window middle    =  " << window_phi_tgc_middle <<
                "  Associated Phi hit         =  " << n_tgc_phi_hits << endreq;
          }
        }
        //TGC inner station
        if (digit.sta == 3)
        {
          double phi_ext_inner = t.PhiExtrapolatedInnerEndCap();
          if (phi_ext_inner<0) phi_ext_inner = phi_ext_inner+2.*M_PI;

          double phi_digit = digit.phi;
          if (phi_digit<0) phi_digit = phi_digit+2.*M_PI;

          //monitoring TGC, inner station, dPhi
          m_hit_tgc_strip_inner_dPhi.push_back(phi_digit - phi_ext_inner);

          if ( m_extrapolator->matchInnerTgcPhiHit(phi_digit, t) )
          {
            n_tgc_phi_hits++;
            digit.associated = true;
          }
          if (msgLvl() <= MSG::DEBUG)
          {
            msg() << MSG::DEBUG << "  Phi of TGC phi Hit         =  " << phi_digit <<
                "  Eta of TGC phi Hit         =  " << digit.eta <<
                "  Phi of extrapolated track  =  " << phi_ext_inner <<
                "  Delta phi                  =  " << fabs(phi_digit - phi_ext_inner) <<
//                 "  Delta phi window middle    =  " << window_phi_tgc_inner <<
                "  Associated Phi hit         =  " << n_tgc_phi_hits << endreq;
          }
        }

      }
      else
      {
        if (digit.sta < 3)
        {
          double phi_ext_middle = t.PhiExtrapolatedMiddleEndCap();
          if (phi_ext_middle<0) phi_ext_middle = phi_ext_middle+2.*M_PI;

          double phi_digit = digit.phi;
          if (phi_digit<0) phi_digit = phi_digit+2.*M_PI;

          //monitoring TGC, middle, dEta and dPhi
          m_hit_tgc_wire_middle_dEta.push_back(digit.eta - t.EtaExtrapolatedMiddleEndCap());
          m_hit_tgc_wire_middle_dPhi.push_back(phi_digit - phi_ext_middle);

          if( m_extrapolator->matchMiddleTgcRhoHit( digit.eta, phi_digit, t) )
          {
            n_tgc_eta_hits++;
            digit.associated = true;
          }
          if (msgLvl() <= MSG::DEBUG)
          {
            msg() << MSG::DEBUG << "  Eta of TGC eta Hit         =  " << digit.eta <<
                "  Phi of TGC eta Hit         =  " << phi_digit <<
                "  Eta of extrapolated track  =  " << t.EtaExtrapolatedMiddleEndCap() <<
                "  Delta eta                  =  " << fabs(digit.eta - t.EtaExtrapolatedMiddleEndCap()) <<
//                 "  Delta eta window middle    =  " << window_eta_tgc_middle <<
                "  Phi of extrapolated track  =  " << phi_ext_middle <<
                "  Delta phi                  =  " << fabs(phi_digit - phi_ext_middle) <<
                "  Delta phi window middle    =  0.2 " <<
                "  Associated Eta hit         =  " << n_tgc_eta_hits << endreq;
          }
        }
        if (digit.sta == 3)
        {
          double phi_ext_inner = t.PhiExtrapolatedInnerEndCap();
          if (phi_ext_inner<0) phi_ext_inner = phi_ext_inner+2.*M_PI;

          double phi_digit = digit.phi;
          if (phi_digit<0) phi_digit = phi_digit+2.*M_PI;

          //monitoring TGC, inner station, dEta and dPhi
          m_hit_tgc_wire_inner_dEta.push_back(digit.eta - t.EtaExtrapolatedInnerEndCap());
          m_hit_tgc_wire_inner_dPhi.push_back(phi_digit - phi_ext_inner);

          if (m_extrapolator->matchInnerTgcRhoHit(digit.eta, phi_digit, t) )
          {
            n_tgc_eta_hits++;
            digit.associated = true;
          }
          if (msgLvl() <= MSG::DEBUG)
          {
            msg() << MSG::DEBUG << "  Eta of TGC eta Hit         =  " << digit.eta <<
                "  Phi of TGC eta Hit         =  " << phi_digit <<
                "  Eta of extrapolated track  =  " << t.EtaExtrapolatedInnerEndCap() <<
                "  Delta eta                  =  " << fabs(digit.eta - t.EtaExtrapolatedInnerEndCap()) <<
//                 "  Delta eta window inner     =  " << window_eta_tgc_inner <<
                "  Phi of extrapolated track  =  " << phi_ext_inner <<
                "  Delta phi                  =  " << fabs(phi_digit - phi_ext_inner) <<
                "  Delta phi window inner     =  0.2 " <<
                "  Associated Eta hit         =  " << n_tgc_eta_hits << endreq;
          }
        }
      }
    }
  }
  else
  {
    for (int iDigit = 0; iDigit < (int)m_TgcDigitVector.size(); iDigit++)
    {
      // Get the digit point.
      Identifier idDigit = m_TgcDigitVector[iDigit]->identify();
      const MuonGM::TgcReadoutElement* pROElement = m_pMuonMgr->getTgcReadoutElement(idDigit);
      assert(pROElement != NULL);
      const Amg::Vector3D pos = m_pTgcIdHelper->isStrip(idDigit) ?
          pROElement->stripPos(idDigit) : pROElement->gangPos(idDigit);

      // Get the station index (station name converted t o0-based number).
      int iStation = pROElement->getStationName()[1] - '1';
      if (m_pTgcIdHelper->isStrip(idDigit))
      {
        // For strips, apply phi cut.
        double phi = pos.phi();
        if (phi < 0.) phi = phi + (2.*M_PI);
        double window_phi_tgc_middle = m_TgcPhiFactor*301.65/pow(t.Pt(),1.0039);

        //monitoring TGC, middle, dPhi
        m_hit_tgc_strip_middle_dPhi.push_back(phi - t.PhiExtrapolatedMiddleEndCap());

        if (iStation < 3 && fabs(phi - t.PhiExtrapolatedMiddleEndCap()) < window_phi_tgc_middle)
          n_tgc_phi_hits++;
        if (msgLvl() <= MSG::DEBUG)
        {
          msg() << MSG::DEBUG << "  Phi of TGC phi Hit         =  " << phi <<
              "  Phi of extrapolated track  =  " << t.PhiExtrapolatedMiddleEndCap() <<
              "  Delta phi                  =  " << fabs(phi - t.PhiExtrapolatedMiddleEndCap()) <<
              "  Delta phi window           =  " << window_phi_tgc_middle <<
              "  Associated Phi hit         =  " << n_tgc_phi_hits << endreq;
        }
      }
      else
      {
        // For wires, apply eta cut.
        double window_eta_tgc_middle = m_TgcEtaFactor*112.7/pow(t.Pt(),0.9595);

        //monitoring TGC, middle, dEta
        m_hit_tgc_wire_middle_dEta.push_back(pos.eta() - t.EtaExtrapolatedMiddleEndCap());

        if (iStation < 3 && fabs(pos.eta() - t.EtaExtrapolatedMiddleEndCap()) < window_eta_tgc_middle)
          n_tgc_eta_hits++;
        if (msgLvl() <= MSG::DEBUG)
        {
          msg() << MSG::DEBUG << "  Eta of TGC eta Hit         =  " << pos.eta() <<
              "  Eta of extrapolated track  =  " << t.EtaExtrapolatedMiddleEndCap() <<
              "  Delta eta                  =  " << fabs(pos.eta() - t.EtaExtrapolatedMiddleEndCap()) <<
              "  Delta eta window           =  " << window_eta_tgc_middle <<
              "  Associated Eta hit         =  " << n_tgc_eta_hits << endreq;
        }
      }
    }
  }

  t.setNTgcEtaHits(n_tgc_eta_hits);
  t.setNTgcPhiHits(n_tgc_phi_hits);

  return;
}

void TrigDiMuonFast::setDetails(const Candidate & t, MuonFeatureDetails* m_details)
{
	//uint32_t muondetmask =  m_hasMdtBarrelA       &
	//                       (m_hasMdtBarrelC << 1) &
	//                       (m_hasMdtEndcapA << 2) &
	//                       (m_hasMdtEndcapC << 3) &
	//                       (m_hasRpcBarrelA << 4) &
	//                       (m_hasRpcBarrelC << 5) &
	//                       (m_hasTgcEndcapA << 6) &
	//                       (m_hasTgcEndcapC << 7) &
	//                       (m_hasCscEndcapA << 8) &
	//                       (m_hasCscEndcapC << 9);

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " In setDetails: " << m_tgc_lut_digits.size() << " TGC hits "
      << m_rpc_raw_data_list.size() << " RPC hits "
      << m_mdt_tubes.size() << " MDT hits "
      << endreq;

	/*m_details = new MuonFeatureDetails( MuonFeatureDetails::NULLID,
  inputTE->getId(),
  m_current_lvl1_id,
  m_current_lbk_id,
  0,
  0,
  m_pMuonRoI->sysID(),
  m_pMuonRoI->subsysID(),
  m_pMuonRoI->sectorID(),
  m_pMuonRoI->getRoINumber(),
  m_pMuonRoI->getThresholdNumber(),
  m_pMuonRoI->eta(),
  m_pMuonRoI->phi(),
  m_esd_ext_size,
  m_esd_rob_size,
  m_esd_csm_size,
  m_esd_lv1_size,
  m_esd_rpc_size,
  m_esd_tgc_size,
  m_esd_mdt_size );
  */

//  if (msgLvl() <= MSG::DEBUG)
//    msg() << MSG::DEBUG << " In setDetails: " << m_tgc_lut_digits.size() << " TGC hits "
//        << m_rpc_raw_data_list.size() << " RPC hits "
//        << m_mdt_tubes.size() << " MDT hits "
//        << endreq;

  if (m_details && (!m_tgc_lut_digits.empty() || !m_rpc_raw_data_list.empty() || !m_mdt_tubes.empty()))
  {
    for (TrigDiMuon::LUTDigitVec::const_iterator itDig  = m_tgc_lut_digits.begin();
         itDig != m_tgc_lut_digits.end(); ++itDig)
    {
      const TrigDiMuon::tgcLUTDigit& digit = *itDig;
      if (digit.sta == 3) // Inner station
      {
        if (digit.isStrip)
          m_details->setTgcInnPhiHit(digit.phi, digit.r, digit.z, digit.width, digit.associated);
        else
          m_details->setTgcInnRhoHit(digit.phi, digit.r, digit.z, digit.width, digit.associated);
      }
      else // Middle station
      {
        if (digit.isStrip)
          m_details->setTgcMidPhihit(digit.phi, digit.r, digit.z, digit.width, digit.associated);
        else
          m_details->setTgcMidRhoHit(digit.phi, digit.r, digit.z, digit.width, digit.associated);
      }
    }
    for (TrigDiMuon::mdtTubeVec::const_iterator itDig  = m_mdt_tubes.begin();
         itDig != m_mdt_tubes.end(); ++itDig)
    {
      const TrigDiMuon::mdtTube& digit = *itDig;
      m_details->setMdtHit(digit.mdt_TdcId, digit.mdt_ChannelId, digit.mdt_tube_r, digit.mdt_tube_z,
                           digit.associated, 0., 0., 0.);
    }
    for (std::vector<RPCrawData>::const_iterator itRaw  = m_rpc_raw_data_list.begin();
         itRaw != m_rpc_raw_data_list.end(); ++itRaw)
    {
      const RPCrawData& rpc_digits = *itRaw;
      for (RPCrawData::DigitList::const_iterator itDig  = rpc_digits.eta_digits().begin();
           itDig != rpc_digits.eta_digits().end(); ++itDig)
      {
        const RPCrawDigit& digit = *itDig;
        float xyz[3], r;
        digit.xyz(xyz); digit.radius(r);
        m_details->setPadHit(0, 0/*eta*/, xyz[0], xyz[1], xyz[2], r, 0.0f);
      }
      for (RPCrawData::DigitList::const_iterator itDig  = rpc_digits.phi_digits().begin();
           itDig != rpc_digits.phi_digits().end(); ++itDig)
      {
        const RPCrawDigit& digit = *itDig;
        float xyz[3], r;
        digit.xyz(xyz); digit.radius(r);
        m_details->setPadHit(0, 1/*phi*/, xyz[0], xyz[1], xyz[2], r, 0.0f);
      }
    }
    // parameters of ID track
    floatToInt Pt;
    Pt.f = (float)t.Pt();
    m_details->setExtension0(Pt.i);
    floatToInt Eta;
    Eta.f = (float)t.Eta();
    m_details->setExtension0(Eta.i);
    floatToInt Phi;
    Phi.f = (float)t.Phi();
    m_details->setExtension0(Phi.i);
    m_details->setExtension0(t.Charge());

    // Extrapolated parameters
    floatToInt EtaExtIE;
    EtaExtIE.f = (float)t.EtaExtrapolatedInnerEndCap();
    m_details->setExtension1(EtaExtIE.i);
    floatToInt PhiExtIE;
    PhiExtIE.f = (float)t.PhiExtrapolatedInnerEndCap();
    m_details->setExtension1(PhiExtIE.i);
    floatToInt EtaExtIB;
    EtaExtIB.f = (float)t.EtaExtrapolatedInnerBarrel();
    m_details->setExtension1(EtaExtIB.i);
    floatToInt PhiExtIB;
    PhiExtIB.f = (float)t.PhiExtrapolatedInnerBarrel();
    m_details->setExtension1(PhiExtIB.i);
    floatToInt EtaExtME;
    EtaExtME.f = (float)t.EtaExtrapolatedMiddleEndCap();
    m_details->setExtension1(EtaExtME.i);
    floatToInt PhiExtME;
    PhiExtME.f = (float)t.PhiExtrapolatedMiddleEndCap();
    m_details->setExtension1(PhiExtME.i);
    floatToInt EtaExtMB;
    EtaExtMB.f = (float)t.EtaExtrapolatedMiddleBarrel();
    m_details->setExtension1(EtaExtMB.i);
    floatToInt PhiExtMB;
    PhiExtMB.f = (float)t.PhiExtrapolatedMiddleBarrel();
    m_details->setExtension1(PhiExtMB.i);

    // Number of hits found
    m_details->setExtension2(t.NMdtHits());
    m_details->setExtension2(t.NTgcEtaHits());
    m_details->setExtension2(t.NTgcPhiHits());
    m_details->setExtension2(t.NRpcEtaHits());
    m_details->setExtension2(t.NRpcPhiHits());

    //if (msgLvl() <= MSG::DEBUG)
    //  msg() << MSG::DEBUG << "Going to attach m_details" << endreq;

        /*HLT::ErrorCode stat = attachFeature(outputTE, m_details);
    if (stat!=HLT::OK)
    {
    msg() << MSG::ERROR << "Record into StoreGate failed" << endreq;
    return;
  }
    if (msgLvl() <= MSG::DEBUG)
    msg()  << MSG::DEBUG << *m_details << endreq;
        */

  }
}


AssociationResult TrigDiMuonFast::processTrack(TrigInDetTrack* track,
                                               int i_track,
                                               int firstOrSecond,
                                               const LVL1::RecMuonRoI* m_pMuonRoI,
                                               const HLT::TriggerElement* inputTE,
                                               HLT::TriggerElement* outputTE)
{

  std::string trackName = "first";

  //for debug outputs:
  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << "In processTrack()" << endreq;
    if(firstOrSecond!=1) trackName = "second";
  }


  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Start the " << trackName << " track assosiation! " << endreq;

  Candidate t;

  if (doTiming())
  {             //timing
    m_timerTotal1->pause();
    m_timerAllFun->resume();
    m_timerPerFun->start();
  }

  //hit association
  int n_mdt_dig = 0, n_rpc_eta_dig = 0, n_rpc_phi_dig = 0, n_tgc_eta_dig = 0, n_tgc_phi_dig = 0;
  bool associated_track = HitAssociation( track, t, n_mdt_dig, n_rpc_eta_dig, n_rpc_phi_dig, n_tgc_eta_dig, n_tgc_phi_dig);

  //debug output
  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << trackName << " associated_track = " << associated_track << endreq;
    msg() << MSG::DEBUG << " parameters of the " << trackName << " associated track "
        << "  i_tr = " << i_track
        << "  Pt     =  " << t.Pt()
        << "  Eta    =  " << t.Eta()
        << "  Phi    =  " << t.Phi()
        << "  Charge =  " << t.Charge()
        << "  NMdtHits = " << t.NMdtHits()
        << "  NTgcEtaHits = " << t.NTgcEtaHits()
        << "  NTgcPhiHits = " << t.NTgcPhiHits()
        << "  NRpcEtaHits = " << t.NRpcEtaHits()
        << "  NRpcPhiHits = " << t.NRpcPhiHits()
        << "  EtaExtrapolatedInnerEndCap =  " << t.EtaExtrapolatedInnerEndCap()
        << "  PhiExtrapolatedInnerEndCap =  " << t.PhiExtrapolatedInnerEndCap()
        << "  EtaExtrapolatedInnerBarrel =  " << t.EtaExtrapolatedInnerBarrel()
        << "  PhiExtrapolatedInnerBarrel =  " << t.PhiExtrapolatedInnerBarrel()
        << "  EtaExtrapolatedMiddleEndCap = " << t.EtaExtrapolatedMiddleEndCap()
        << "  PhiExtrapolatedMiddleEndCap = " << t.PhiExtrapolatedMiddleEndCap()
        << "  EtaExtrapolatedMiddleBarrel = " << t.EtaExtrapolatedMiddleBarrel()
        << "  PhiExtrapolatedMiddleBarrel = " << t.PhiExtrapolatedMiddleBarrel() << endreq;
  }

      //convert number of hits to word and update hit pattern
  int32_t word = track->HitPattern();
  Encoding* pEncoding1 = (Encoding*)&word;

  if (t.NMdtHits()<31)
    pEncoding1->mdt = t.NMdtHits();
  else
    pEncoding1->mdt = 31;

  int tgcH = t.NTgcEtaHits()+t.NTgcPhiHits();
  if (tgcH<15)
    pEncoding1->tgc = tgcH;
  else
    pEncoding1->tgc = 15;

  int rpcH = t.NRpcEtaHits()+t.NRpcPhiHits();
  if (rpcH<15)
    pEncoding1->rpc = rpcH;
  else
    pEncoding1->rpc = 15;

  if (msgLvl() <= MSG::DEBUG)
  {
    msg() << MSG::DEBUG << " word " << trackName << " =  " << word << endreq;
  }
  //update hit pattern
  track->HitPattern(word);

  //create muon feature datail:
  MuonFeatureDetails* details = NULL;
  if (m_fill_esd  && (!m_tgc_lut_digits.empty() || !m_rpc_raw_data_list.empty() || !m_mdt_tubes.empty()))
  {
    int roi_sysID     = 0;
    int roi_subsysID  = 0;
    int rot_sectorID  = 0;
    int roi_number    = 0;
    int roi_threshold = 0;
    double roi_eta    = 0;
    double roi_phi    = 0;

    if(m_retrieveRoI && m_pMuonRoI!=NULL) {
      roi_sysID     = m_pMuonRoI->sysID();
      roi_subsysID  = m_pMuonRoI->subsysID();
      rot_sectorID  = m_pMuonRoI->sectorID(),
      roi_number    = m_pMuonRoI->getRoINumber(),
      roi_threshold = m_pMuonRoI->getThresholdNumber();
      roi_eta       = m_pMuonRoI->eta();
      roi_phi       = m_pMuonRoI->phi();
    }

    details = new MuonFeatureDetails( MuonFeatureDetails::NULLID, inputTE->getId(),
                                        m_current_lvl1_id,
                                        m_current_lbk_id,
                                        0,
                                        0,
                                        roi_sysID,
                                        roi_subsysID,
                                        rot_sectorID,
                                        roi_number,
                                        roi_threshold,
                                        roi_eta,
                                        roi_phi,
                                        m_esd_ext_size,
                                        m_esd_rob_size,
                                        m_esd_csm_size,
                                        m_esd_lv1_size,
                                        m_esd_rpc_size,
                                        m_esd_tgc_size,
                                        m_esd_mdt_size );
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "############### created " << trackName << " details: i_track = " << i_track << endreq;

    setDetails(t, details);

    //attach feature
    if (msgLvl() <= MSG::DEBUG)
    {
      msg() << MSG::DEBUG << "Going to attach " << trackName << " details" << endreq;
      msg() << MSG::DEBUG << *details << endreq;
    }

    HLT::ErrorCode stat = attachFeature(outputTE, details);
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "############## attached << " << trackName << " details: i_track = " << i_track << endreq;
    if (stat!=HLT::OK)
    {
      m_hist_error.push_back(TrigDiMuonErr::muDetailsFails);
      msg() << MSG::ERROR << "Record into StoreGate failed" << endreq;
    }
  }

  //save extrapolated parameters
//   if(track->endParam() == NULL)
//   {
//     const TrigInDetTrackFitPar * fit_par = new TrigInDetTrackFitPar(
//         t.EtaExtrapolatedInnerEndCap(),
//         t.PhiExtrapolatedInnerEndCap(),
//         t.EtaExtrapolatedInnerBarrel(),
//         t.PhiExtrapolatedInnerBarrel(),
//         t.EtaExtrapolatedMiddleEndCap(),
//         t.PhiExtrapolatedMiddleEndCap(),
//         t.EtaExtrapolatedMiddleBarrel(),
//         t.PhiExtrapolatedMiddleBarrel(),
//         0,
//         0,
//         NULL);
//
//     track->endParam(fit_par);
//   }

  // monitoring: muon track ////////////
  if((!m_tgc_lut_digits.empty() || !m_rpc_raw_data_list.empty() || !m_mdt_tubes.empty()))
  {
    if (track->param()->pT()>0)
    {
      // match track to positive
      m_pos_muon_eta.push_back(track->param()->eta()); // mouns Basics
      m_pos_muon_phi.push_back(track->param()->phi0());
      m_pos_muon_pt.push_back(track->param()->pT());

      if (track->param()->eta() < 1.1)
      {
        //barrel
        m_pos_muon_barrel_mdt_hits.push_back(n_mdt_dig);
        m_pos_muon_rpc_hits.push_back(n_rpc_eta_dig+n_rpc_phi_dig);
      }

      if (track->param()->eta() > 0.9)
      {
        //endcap
        m_pos_muon_cap_mdt_hits.push_back(n_mdt_dig);
        m_pos_muon_tgc_hits.push_back(n_tgc_eta_dig+n_tgc_phi_dig);  //Detector hits information
      }
    }else{
      // match track to negative
      m_neg_muon_eta.push_back(track->param()->eta()); // mouns Basics
      m_neg_muon_phi.push_back(track->param()->phi0());
      m_neg_muon_pt.push_back(fabs(track->param()->pT()));

      if (track->param()->eta() < 1.1)
      {
        // barrel
        m_neg_muon_barrel_mdt_hits.push_back(n_mdt_dig);
        m_neg_muon_rpc_hits.push_back(n_rpc_eta_dig+n_rpc_phi_dig);
      }

      if (track->param()->eta() > 0.9)
      {
        //endcap
        m_neg_muon_cap_mdt_hits.push_back(n_mdt_dig);
        m_neg_muon_tgc_hits.push_back(n_tgc_eta_dig+n_tgc_phi_dig);  //Detector hits information
      }
    }
  }

  if(m_retrieveRoI) {
    // monitoring RoI association:
    if(associated_track)
    {
      double endcap_dEta, endcap_dPhi,  endcap_dR ,barrel_dEta,  barrel_dPhi, barrel_dR;
      double endcap_ne_dEta, endcap_ne_dPhi,  endcap_ne_dR ,barrel_ne_dEta,  barrel_ne_dPhi, barrel_ne_dR;
      getDEtaDPhiDR(track, m_pMuonRoI,
                    endcap_dEta, endcap_dPhi, endcap_dR, barrel_dEta, barrel_dPhi, barrel_dR,
                    endcap_ne_dEta, endcap_ne_dPhi, endcap_ne_dR, barrel_ne_dEta, barrel_ne_dPhi, barrel_ne_dR);

      if( endcap_dEta >= 0) m_roiEndcap_dEta.push_back(endcap_dEta);
      if( endcap_dPhi >= 0) m_roiEndcap_dPhi.push_back(endcap_dPhi);
      if( endcap_dR >= 0)   m_roiEndcap_dR.push_back(endcap_dR);
      if( barrel_dEta >= 0) m_roiBarrel_dEta.push_back(barrel_dEta);
      if( barrel_dPhi >= 0) m_roiBarrel_dPhi.push_back(barrel_dPhi);
      if( barrel_dR >= 0)   m_roiBarrel_dR.push_back(barrel_dR);

      if( endcap_ne_dEta >= 0) m_roiEndcap_ne_dEta.push_back(endcap_ne_dEta);
      if( endcap_ne_dPhi >= 0) m_roiEndcap_ne_dPhi.push_back(endcap_ne_dPhi);
      if( endcap_ne_dR >= 0)   m_roiEndcap_ne_dR.push_back(endcap_ne_dR);
      if( barrel_ne_dEta >= 0) m_roiBarrel_ne_dEta.push_back(barrel_ne_dEta);
      if( barrel_ne_dPhi >= 0) m_roiBarrel_ne_dPhi.push_back(barrel_ne_dPhi);
      if( barrel_ne_dR >= 0)   m_roiBarrel_ne_dR.push_back(barrel_ne_dR);
    }

    //monitoring: tracks that have associated RoI but were not identified as muons
    if(roiAssociation(track, m_pMuonRoI) && !associated_track)
    {
      m_roiNoHits_pt.push_back(fabs(track->param()->pT()));
      m_roiNoHits_charge.push_back( track->param()->pT() > 0 ? 1. : -1. );
      m_roiNoHits_eta.push_back(track->param()->eta());
      m_roiNoHits_phi.push_back(track->param()->phi0());
    }
  }

  if (doTiming())
  {
    m_timerTotal1->resume();
    m_timerAllFun->pause();
    m_timerPerFun->stop();
  }

  //determine track status:
  if(m_tgc_lut_digits.empty() && m_rpc_raw_data_list.empty() && m_mdt_tubes.empty())
    return NO_HITS_ON_ROD;
  else if(!associated_track)
    return MUON_ID_FAILED;
  else
    return MUON_ID_SUCCESSFUL;

}

void TrigDiMuonFast::tagAndProbe(std::vector<std::pair<TrigInDetTrack*, AssociationResult> >& myCollection,
                                 const LVL1::RecMuonRoI* m_pMuonRoI,
                                 const HLT::TriggerElement* inputTE,
                                 HLT::TriggerElement* outputTE)
{

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "In tagAndProbe()" << endreq;

  //loop over first track
  for (int i_tr1 = 0; i_tr1 < (int)myCollection.size(); ++i_tr1)
  {
    TrigInDetTrack * tr1 = myCollection[i_tr1].first;
    AssociationResult associated_tr1 = myCollection[i_tr1].second;


    double pt_idtr1 = tr1->param()->pT();
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " pt1 = " << pt_idtr1 << endreq;

    //begin the loop over the second track
    bool skip = false;
    for (int i_tr2 = i_tr1 + 1; i_tr2 < (int)myCollection.size() && !skip; ++i_tr2)
    {

      TrigInDetTrack * tr2 = myCollection[i_tr2].first;
      AssociationResult associated_tr2 = myCollection[i_tr2].second;

      double pt_idtr2 = tr2->param()->pT();
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << " pt2 = " << pt_idtr2 << endreq;

      if(!m_doOppChargeCheck)
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Opposite charge check disabled" << endreq;

      // calculate invariant mass
      double mass = invariant_mass(   fabs(pt_idtr1),
                                      tr1->param()->phi0(),
                                      tr1->param()->eta(),
                                      fabs(pt_idtr2),
                                      tr2->param()->phi0(),
                                      tr2->param()->eta());

      //cut on invariant mass
      if (mass < m_JpsiMassMin) continue;

      // Hit association: find hits on track. Will be called only once per track
      //track 1: ************************************************************************
      if(associated_tr1 == UNKNOWN)
      {
        associated_tr1 = processTrack(tr1, i_tr1, 1, m_pMuonRoI, inputTE, outputTE);
        myCollection[i_tr1].second = associated_tr1;
      }

      // RoI association:
      bool roiAssociated_tr1 = roiAssociation(tr1, m_pMuonRoI);

      //track 2: ****************************************************************************
      if(associated_tr2 == UNKNOWN)
      {
        associated_tr2 = processTrack(tr2, i_tr2, 2, m_pMuonRoI, inputTE, outputTE);
        myCollection[i_tr2].second = associated_tr2;
      }

      // RoI association:
      bool roiAssociated_tr2 = roiAssociation(tr2, m_pMuonRoI);

      //*************************************************************************************

      // Tag and probe histograms:
      // Histograms are filled with invariant mass of di-track

      // 1/ all di-tracks
      m_tap_mass_trTr.push_back(mass);
      if(pt_idtr1*pt_idtr2 < 0) m_tap_mass_trTr_os.push_back(mass);

      //opposite charge check
      if (m_doOppChargeCheck && pt_idtr1*pt_idtr2 > 0)
      {
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Pair rejected by opposite charge check" << endreq;
        continue;
      }

      // 2/ one track must be matched to RoI (tag), the other will be used as probe:
      //    NOTE: if both tracks are matched to RoI entry will be added twice to avoid bias!
      if(roiAssociated_tr1) m_tap_mass_trRoI.push_back(mass);
      if(roiAssociated_tr2) m_tap_mass_trRoI.push_back(mass);

      // 3/ the probe track must be identified as muon
      if(roiAssociated_tr1 && associated_tr2==MUON_ID_SUCCESSFUL) m_tap_mass_muRoI.push_back(mass);
      if(roiAssociated_tr2 && associated_tr1==MUON_ID_SUCCESSFUL) m_tap_mass_muRoI.push_back(mass);

      // 4/ both tag and probe tracks must be confirmed muons: this histogram should be in theory
      //    the same as the previous one
      if(associated_tr1==MUON_ID_SUCCESSFUL && associated_tr2==MUON_ID_SUCCESSFUL)
      {
        if(roiAssociated_tr1) m_tap_mass_muMuRoI.push_back(mass);
        if(roiAssociated_tr2) m_tap_mass_muMuRoI.push_back(mass);
      }
    }
  }
}

void TrigDiMuonFast::getDEtaDPhiDR(TrigInDetTrack* track,
                                   const LVL1::RecMuonRoI* m_pMuonRoI,
                                   double& endcap_dEta, double& endcap_dPhi, double& endcap_dR,
                                   double& barrel_dEta, double& barrel_dPhi, double& barrel_dR,
                                   double& endcap_ne_dEta, double& endcap_ne_dPhi, double& endcap_ne_dR,
                                   double& barrel_ne_dEta, double& barrel_ne_dPhi, double& barrel_ne_dR  )
{

  Candidate t;
  t.setParameters(fabs(track->param()->pT()), track->param()->eta(), track->param()->phi0(), (int)(track->param()->pT()/fabs(track->param()->pT())));

  //extrapolate
  m_extrapolator->propagationMiddleBarrel(t);
  m_extrapolator->propagationMiddleEndCap(t);

  double etaEndcap = t.EtaExtrapolatedMiddleEndCap();
  double phiEndcap = t.PhiExtrapolatedMiddleEndCap();

  double etaBarrel = t.EtaExtrapolatedMiddleBarrel();
  double phiBarrel = t.PhiExtrapolatedMiddleBarrel();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Extrapolated parameters (middle station): "
      << " etaEndcap = "
      << etaEndcap
      << " phiEndcap = "
      << phiEndcap
      << " etaBarrel = "
      << etaBarrel
      << " phiBarrel = "
      << phiBarrel
      << endreq;

  endcap_dEta = -1.;  endcap_dPhi = -1.;  endcap_dR = -1.;
  barrel_dEta = -1.;  barrel_dPhi = -1.;  barrel_dR = -1.;
  endcap_ne_dEta = -1.;  endcap_ne_dPhi = -1.;  endcap_ne_dR = -1.;
  barrel_ne_dEta = -1.;  barrel_ne_dPhi = -1.;  barrel_ne_dR = -1.;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "DANN: phi of RoI = " << m_pMuonRoI->phi() <<
      ", phi of track = " << track->param()->phi0() <<
      ", phi extrapoladed to barrel = " << phiBarrel <<
      ", phi extrapoladed to endcap = " << phiEndcap << endreq;

  //endacap
  if(track->param()->eta() > 0.9)
  {
    //extrapolation
    endcap_dEta = fabs( etaEndcap - m_pMuonRoI->eta());
    endcap_dPhi = fabs( phiEndcap - m_pMuonRoI->phi());
    if (msgLvl() <= MSG::DEBUG)
      if(endcap_dPhi > 2*3.14159) msg() << MSG::DEBUG << "BUGGER: dPhi  too large" << endreq;

    if(endcap_dPhi > 3.14159) endcap_dPhi = 2*3.14159 - endcap_dPhi;
    endcap_dR = sqrt(pow(endcap_dEta, 2) + pow(endcap_dPhi, 2));
    //no extrapolation
    endcap_ne_dEta = fabs( track->param()->eta()  - m_pMuonRoI->eta());
    endcap_ne_dPhi = fabs( track->param()->phi0() - m_pMuonRoI->phi());
    if (msgLvl() <= MSG::DEBUG)
      if(endcap_ne_dPhi > 2*3.14159) msg() << MSG::DEBUG << "BUGGER: dPhi  too large" << endreq;

    if(endcap_ne_dPhi > 3.14159) endcap_ne_dPhi = 2*3.14159 - endcap_ne_dPhi;
    endcap_ne_dR = sqrt(pow(endcap_dEta, 2) + pow(endcap_dPhi, 2));
  }
  //barrel
  if(track->param()->eta() < 1.1)
  {
    barrel_dEta = fabs( etaBarrel - m_pMuonRoI->eta());
    barrel_dPhi = fabs( phiBarrel - m_pMuonRoI->phi());
    if (msgLvl() <= MSG::DEBUG)
      if(barrel_dPhi > 2*3.14159) msg() << MSG::DEBUG << "BUGGER: dPhi  too large" << endreq;

    if(barrel_dPhi > 3.14159) barrel_dPhi = 2*3.14159 - barrel_dPhi;
    barrel_dR = sqrt(pow(barrel_dEta, 2) + pow(barrel_dPhi, 2));
    //no extrapolation
    barrel_ne_dEta = fabs( track->param()->eta()  - m_pMuonRoI->eta());
    barrel_ne_dPhi = fabs( track->param()->phi0() - m_pMuonRoI->phi());
    if (msgLvl() <= MSG::DEBUG)
      if(barrel_ne_dPhi > 2*3.14159) msg() << MSG::DEBUG << "BUGGER: dPhi  too large" << endreq;

    if(barrel_ne_dPhi > 3.14159) barrel_ne_dPhi = 2*3.14159 - barrel_ne_dPhi;
    barrel_ne_dR = sqrt(pow(barrel_ne_dEta, 2) + pow(barrel_ne_dPhi, 2));
  }

}

bool TrigDiMuonFast::roiAssociation(TrigInDetTrack* track, const LVL1::RecMuonRoI* m_pMuonRoI)
{

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "In roiAssociation()" << endreq;

//   if(track->endParam() == NULL)
//   {
//     msg() << MSG::WARNING << "RoI association was called before extrapolation" << endreq;
//     return false;
//   }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Track to be associated with RoI: "
      << " pt = "
      <<   track->param()->pT()
      << " eta = "
      <<   track->param()->eta()
      << " phi = "
      <<   track->param()->phi0()
      << endreq;


  double endcap_dEta, endcap_dPhi,  endcap_dR ,barrel_dEta,  barrel_dPhi, barrel_dR;
  double endcap_ne_dEta, endcap_ne_dPhi,  endcap_ne_dR ,barrel_ne_dEta,  barrel_ne_dPhi, barrel_ne_dR;
  getDEtaDPhiDR(track, m_pMuonRoI,
                endcap_dEta, endcap_dPhi, endcap_dR, barrel_dEta, barrel_dPhi, barrel_dR,
                endcap_ne_dEta, endcap_ne_dPhi, endcap_ne_dR, barrel_ne_dEta, barrel_ne_dPhi, barrel_ne_dR);

  //apply cuts
  bool accept = false;
  //in endcap
  if(track->param()->eta() > 0.9)
  {

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " RoI association in endcap: dEta = " << endcap_dEta << ", dPhi = " << endcap_dPhi << endreq;

    //if(endcap_dEta < m_roiEndcap_dEtaCut && endcap_dPhi < m_roiEndcap_dPhiCut ) accept = true;
    if(endcap_ne_dEta < m_roiEndcap_dEtaCut && endcap_ne_dPhi < m_roiEndcap_dPhiCut ) accept = true;

  }

  //in barrel
  if(track->param()->eta() < 1.1)
  {

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " RoI association in barrel: dEta = " << barrel_dEta << ", dPhi = " << barrel_dPhi << endreq;

    //if(barrel_dEta < m_roiBarrel_dEtaCut && barrel_dPhi < m_roiBarrel_dPhiCut ) accept = true;
    if(barrel_ne_dEta < m_roiBarrel_dEtaCut && barrel_ne_dPhi < m_roiBarrel_dPhiCut ) accept = true;
  }

  return accept;

}

