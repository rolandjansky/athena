/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtRawDataValAlg
// Authors:   N. Benekos(Illinois) 
//            A. Cortes (Illinois) 
//            G. Dedes (MPI) 
//            Orin Harris (University of Washington)
//            Justin Griffiths (University of Washington)
// Oct. 2007
//
// DESCRIPTION:
// Subject: MDT-->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MdtCalibFitters/MTStraightLine.h"
#include "MuonSegment/MuonSegment.h"

#include "MdtRawDataMonitoring/MuonChamberIDSelector.h"
#include "MdtRawDataMonitoring/MdtRawDataValAlg.h"
#include "TrkEventPrimitives/FitQuality.h"
//#include "xAODEventInfo/EventInfo.h"
//#include "xAODMuon/MuonContainer.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "xAODTrigger/MuonRoIContainer.h"
// #include "GaudiKernel/Property.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "GaudiKernel/MsgStream.h"

//root includes
#include <TH1.h>
#include <TH2.h> 
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TF1.h>
#include <TError.h>
#include <math.h>
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include <sstream>

using namespace std;

float parESD1, parESD2, parESD3, parESD4;

enum {enumBarrelA, enumBarrelC, enumEndCapA, enumEndCapC};
enum {enumBarrel, enumEndCap};
enum {enumInner, enumMiddle, enumOuter, enumExtra};

/////////////////////////////////////////////////////////////////////////////
// *********************************************************************
// Public Methods
// ********************************************************************* 

MdtRawDataValAlg::MdtRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
:ManagedMonitorToolBase( type, name, parent ),
 m_mg(0),
 m_masked_tubes(NULL),
 m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
 m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
 m_atlas_ready(0),
 m_trig_BARREL(false),
 m_trig_ENDCAP(false),
 m_overalltdcadcLumi(0),
 m_overalltdccutLumi(0),
 m_overalltdccut_segm_Lumi(0),
 m_overalladc_segm_Lumi(0),
 m_overalladc_Lumi(0),
 //overalltdccut_RPCtrig(0),
 //overalltdccut_TGCtrig(0),
 m_overalltdcadcHighOcc(0),
 m_overalltdcHighOcc(0),
 m_overalltdcHighOcc_ADCCut(0),
 m_overalladc_HighOcc(0),
 m_overall_mdt_DRvsDT(0),
 m_overall_mdt_DRvsSegD(0),
 m_MdtNHitsvsRpcNHits(0),
 m_mdteventscutLumi(0),
 m_mdteventscutLumi_big(0),
 m_mdteventsLumi(0),
 m_mdteventsLumi_big(0),
 //mdthitsvseventnum(0),
 //mdthitsvseventnumcut(0),
 m_mdtglobalhitstime(0),
 m_nummdtchamberswithhits(0),
 m_nummdtchamberswithhits_ADCCut(0),
 m_nummdtchamberswithHighOcc(0),
 m_mdtchamberstat(0),
 m_hist_hash_list(0),
 m_BMGpresent(false),
 m_BMGid(-1)
{
  //  Declare the properties
  declareProperty("DoMdtEsd",                m_doMdtESD=false);
  declareProperty("DoChamber2DHist",         m_chamber_2D=true);
  declareProperty("DoChamberHist",           m_doChamberHists=true);
  declareProperty("DoOnlineMonitoring",      m_isOnline=false);
  declareProperty("maskNoisyTubes",          m_maskNoisyTubes=true);
  declareProperty("ChamberName",             m_chamberName="XXX");
  declareProperty("StationSize",             m_StationSize="XXX");
  declareProperty("StationEta",              m_StationEta=-100);
  declareProperty("StationPhi",              m_StationPhi=-100);
  declareProperty("ADCCut",                  m_ADCCut=50.);
  declareProperty("ADCCutForBackground",     m_ADCCut_Bkgrd=80.);
//  declareProperty("MaxTDCCutForBackground",  m_TDCCut_Bkgrd=50.);
//  declareProperty("RPCTrigKey",              m_RPCKey=144);
//  declareProperty("TGCTrigKey",              m_TGCKey=136);
  declareProperty("MdtPrepDataContainer",    m_key_mdt="MDT_DriftCircles");
  declareProperty("Eff_nHits",               m_nb_hits=5.);
  declareProperty("Eff_roadWidth",           m_road_width=2.);
  declareProperty("Eff_chi2Cut",             m_chi2_cut=10.);
  declareProperty("Eff_segm_type",           m_segm_type="MuonSegments");
  declareProperty("AtlasFilterTool",         m_DQFilterTools);
  declareProperty("Title",                   m_title);
  //Global Histogram controls
  declareProperty("do_mdtChamberHits",       m_do_mdtChamberHits = true);
  declareProperty("do_mdttdccut_sector",     m_do_mdttdccut_sector = true);
  declareProperty("do_mdtchamberstatphislice", m_do_mdtchamberstatphislice = true);
  //Chamber wise histogram control
  declareProperty("do_mdttdc",               m_do_mdttdc = true);
  declareProperty("do_mdttdccut_ML1",        m_do_mdttdccut_ML1 = true);
  declareProperty("do_mdttdccut_ML2",        m_do_mdttdccut_ML2 = true);
  declareProperty("do_mdtadc_onSegm_ML1",        m_do_mdtadc_onSegm_ML1 = true);
  declareProperty("do_mdtadc_onSegm_ML2",        m_do_mdtadc_onSegm_ML2 = true);
  declareProperty("do_mdttdccut_RPCtrig_ML1", m_do_mdttdccut_RPCtrig_ML1 = true);
  declareProperty("do_mdttdccut_TGCtrig_ML1", m_do_mdttdccut_TGCtrig_ML1 = true);
  declareProperty("do_mdttdccut_RPCtrig_ML2", m_do_mdttdccut_RPCtrig_ML2 = true);
  declareProperty("do_mdttdccut_TGCtrig_ML2", m_do_mdttdccut_TGCtrig_ML2 = true);
  declareProperty("do_mdtadc",               m_do_mdtadc = true);
  declareProperty("do_mdttdcadc",            m_do_mdttdcadc = true);
  declareProperty("do_mdtmultil",            m_do_mdtmultil = true);
  declareProperty("do_mdtlayer",             m_do_mdtlayer = true);
  declareProperty("do_mdttube",              m_do_mdttube = true);
  declareProperty("do_mdttube_bkgrd",        m_do_mdttube_bkgrd = true);
  declareProperty("do_mdttube_fornoise",     m_do_mdttube_fornoise = true);
  declareProperty("do_mdttube_masked",       m_do_mdttube_masked = true);
  declareProperty("do_mdtmezz",              m_do_mdtmezz = true);
  declareProperty("do_mdt_effEntries",       m_do_mdt_effEntries = true);
  declareProperty("do_mdt_effCounts",        m_do_mdt_effCounts = true);
  declareProperty("do_mdt_effPerTube",       m_do_mdt_effPerTube = false);
  declareProperty("do_mdt_DRvsDT",           m_do_mdt_DRvsDT = true);
  declareProperty("do_mdt_DRvsDRerr",        m_do_mdt_DRvsDRerr = false);
  declareProperty("do_mdt_DRvsSegD",         m_do_mdt_DRvsSegD = true);
  declareProperty("do_mdttubenoise",         m_do_mdttubenoise = false); 
  declareProperty("do_mdttdctube",           m_do_mdttdctube = false);   
  declareProperty("nHits_NoiseThreshold",    m_HighOccThreshold = 10000.);
  declareProperty("RpcPrepDataContainer",    m_key_rpc="RPC_Measurements");
}

MdtRawDataValAlg::~MdtRawDataValAlg()
{
  // fixes for Memory leak
  if(m_hist_hash_list) {
    clear_hist_map(0);
  }
  delete m_masked_tubes; m_masked_tubes = NULL;
  ATH_MSG_INFO(" deleting MdtRawDataValAlg " );
  delete m_mg;

}

/*---------------------------------------------------------*/
StatusCode MdtRawDataValAlg::initialize()
/*---------------------------------------------------------*/
{

  //initialize to stop coverity bugs
   m_activeStore = 0;
   m_mdtIdHelper=0;
   p_MuonDetectorManager=0;
   //mdtevents_RPCtrig = 0;
   //mdtevents_TGCtrig=0;
   m_MdtNHitsvsRpcNHits = 0;
   m_lumiblock = 0;
   m_eventNum = 0;
   m_time = 0;
   m_firstEvent = 0;
   m_firstTime = 0;
   m_segms = 0;
   m_trigtype = 0;
   m_numberOfEvents = 0;
  for(unsigned int i=0;i<4;i++){
     m_overalltdcadcPRLumi[i]=0;
     m_overalltdccut_segm_PR_Lumi[i]=0;
     m_overalladc_segm_PR_Lumi[i]=0;
     m_overalladcPRLumi[i]=0; 
     m_overalladccutPRLumi[i]=0;  
     m_overalltdccutPRLumi_RPCtrig[i]=0;
     m_overalltdccutPRLumi_TGCtrig[i]=0;
     m_overallPR_mdt_DRvsDT[i]=0;
     m_overallPR_mdt_DRvsSegD[i]=0;
     m_mdteffperchamber_InnerMiddleOuter[i]=0;
     m_overalladcPR_HighOcc[i]=0; 
     m_overalltdcPR_HighOcc[i]=0; 
     m_overalltdcadcPR_HighOcc[i]=0; 
     m_overalltdcPR_HighOcc_ADCCut[i]=0; 
     
     if(i==3) continue;
     m_mdtxydet[i]=0;
     m_mdtrzdet[i]=0;
     m_mdthitsperML_byLayer[i]=0;
     if(i==2) continue;
     m_mdthitsperchamber_InnerMiddleOuterLumi[i]=0;
     m_mdthitsperchamber_InnerMiddleOuter_HighOcc[i]=0;
     m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[i]=0;
  }
     for(unsigned int i=0;i<16;i++){
     m_mdtchamberstatphislice[i]=0;
     }


  
  
  // init message stream
  ATH_MSG_INFO("initialize MdtRawDataValAlg" );

  ATH_MSG_DEBUG("******************" );
  ATH_MSG_DEBUG("doMdtESD: " << m_doMdtESD );
  ATH_MSG_DEBUG("******************" );

  StatusCode sc;

  //If Online ensure that lowStat histograms are made at the runLevel and that _lowStat suffix is suppressed
  if(m_isOnline) m_mg = new MDTMonGroupStruct(this, m_title, ManagedMonitorToolBase::run, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "");
  else m_mg = new MDTMonGroupStruct(this, m_title, ManagedMonitorToolBase::lowStat, ManagedMonitorToolBase::MgmtAttr_t::ATTRIB_UNMANAGED, "_lowStat");

  //If online monitoring turn off chamber by chamber hists
  if(m_isOnline) m_doChamberHists = false;

  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get ActiveStoreSvc " );
    return sc ;
  }

  std::string managerName="Muon";
  sc = detStore()->retrieve(p_MuonDetectorManager);
  if (sc.isFailure()) {
    ATH_MSG_INFO("Could not find the MuonGeoModel Manager: " << managerName << " ! " );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the MuonGeoModel Manager " );
  }

  sc = detStore()->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Cannot get MdtIdHelper" );
    return StatusCode::FAILURE;
  }  
  else {
    ATH_MSG_DEBUG(" Found the MdtIdHelper. " );
  }


  if (m_idHelper.retrieve().isFailure()){
    ATH_MSG_WARNING("Could not get " << m_idHelper); 
    return StatusCode::FAILURE;
  }  else {
    ATH_MSG_DEBUG(" Found the MuonIdHelper. " );
  }  
  
  sc = m_DQFilterTools.retrieve();
  if( !sc ) {
    ATH_MSG_ERROR("Could Not Retrieve AtlasFilterTool " << m_DQFilterTools);
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_muonSelectionTool.retrieve());

  //   ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  //   sc = m_trigDec.retrieve();
  //   if (sc.isFailure()) {
  //     mLog << MSG::WARNING << " TriggerDecision not found in SG" << endmsg;
  //     return StatusCode::FAILURE;
  //   }
  //   const Trig::ChainGroup* m_all = m_trigDec->getChainGroup(".*");
  //   std::vector<std::string> AllItems = m_all->getListOfTriggers();
  //   //trigResults.at(i).counts += (int)(m_trigDec->isPassed(AllItems.at(i),TrigDefs::Physics));


  //   //NEW
  //   sc = serviceLocator()->service("RegionSelectionSvc", m_regionSelector);
  //   if (sc.isFailure()) {
  //     ATH_MSG_ERROR("Could not retrieve the region selector service" << m_regionSelector<<". Exiting." );
  //     return sc;
  //   }
  //   else {
  //     ATH_MSG_DEBUG("Retrieved tool "<< m_regionSelector);
  //   } 

  /*sc = detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
  {
    ATH_MSG_ERROR ( "Can't retrieve RpcIdHelper" );
    return sc;
  }

  // get RPC cablingSvc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  sc = service("RPCcablingServerSvc", RpcCabGet);
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Could not get RPCcablingServerSvc !" );
    return StatusCode::FAILURE;
  }*/

   //back to MDTS... 
  if(m_maskNoisyTubes) m_masked_tubes = new MDTNoisyTubes();
  else m_masked_tubes = new MDTNoisyTubes(false);
  mdtchamberId();

  //m_booked = false;

  ManagedMonitorToolBase::initialize().ignore();  //  Ignore the checking code;

  m_BMGpresent = m_mdtIdHelper->stationNameIndex("BMG") != -1;
  if(m_BMGpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
    m_BMGid = m_mdtIdHelper->stationNameIndex("BMG");
    for(int phi=6; phi<8; phi++) { // phi sectors
      for(int eta=1; eta<4; eta++) { // eta sectors
        for(int side=-1; side<2; side+=2) { // side
          if( !p_MuonDetectorManager->getMuonStation("BMG", side*eta, phi) ) continue;
          for(int roe=1; roe<=( p_MuonDetectorManager->getMuonStation("BMG", side*eta, phi) )->nMuonReadoutElements(); roe++) { // iterate on readout elemets
            const MuonGM::MdtReadoutElement* mdtRE =
                  dynamic_cast<const MuonGM::MdtReadoutElement*> ( ( p_MuonDetectorManager->getMuonStation("BMG", side*eta, phi) )->getMuonReadoutElement(roe) ); // has to be an MDT
            if(mdtRE) initDeadChannels(mdtRE);
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------------------------*/
StatusCode MdtRawDataValAlg::bookHistogramsRecurrent( /*bool isNewEventsBlock, bool newLumiBlockFlag(), bool newRunFlag() */)
/*----------------------------------------------------------------------------------*/
{
  //changed for booking unmanaged histograms for MIG2

  StatusCode sc = StatusCode::SUCCESS;

  if(newRunFlag() || newLowStatIntervalFlag()){
    sc = bookMDTSummaryHistograms(/* isNewEventsBlock,*/ newLumiBlockFlag(), newRunFlag());
    if(sc.isFailure()) { 
      ATH_MSG_FATAL("Failed to bookMDTSummaryHistograms" );
      return sc;
    }
    sc = bookMDTOverviewHistograms(/* isNewEventsBlock,*/ newLumiBlockFlag(), newRunFlag());
    if(sc.isFailure()) { 
      ATH_MSG_FATAL("Failed to bookMDTOverviewHistograms" );
      return sc;
    }  
  }

  //if(isNewEventsBlock) {}
  if(newLumiBlockFlag()) {}
  if(newRunFlag()) {      
    ATH_MSG_DEBUG("MDT RawData Monitoring from ESD : newRunFlag()" );    
    //Book All Chambers
    //Protection against newRunFlag()()
    clear_hist_map();
    int counter = 0;
      sc= GetTimingInfo();
        if (sc.isFailure()){
          ATH_MSG_ERROR(" Cannot GetTimingInfo " );
        return sc;
       }
     m_firstTime = m_time;
    //std::cout << "m_firstTime listed as: " << m_firstTime << std::endl; 
 
    sc= GetEventNum();
                        if (sc.isFailure()){
                          ATH_MSG_ERROR(" Cannot GetEventNum " );
                  return sc;
                 }
     m_firstEvent = m_eventNum;

     ATH_MSG_DEBUG("MdtRawDataValAlg::MDT RawData Monitoring Histograms being filled" );

    for(vector<Identifier>::const_iterator itr = m_chambersId.begin(); itr != m_chambersId.end(); ++itr, ++counter){
      std::string hardware_name = convertChamberName(m_mdtIdHelper->stationName(*itr),m_mdtIdHelper->stationEta(*itr),
          m_mdtIdHelper->stationPhi(*itr),"MDT");
      //Skip Chambers That Do NOT Exist
      if(hardware_name=="BML6A13" || hardware_name=="BML6C13") continue;

      MDTChamber* chamber = new MDTChamber(hardware_name);
      (*m_hist_hash_list)[m_chambersIdHash.at(counter)] = chamber;
      m_hardware_name_list.insert(hardware_name);
      chamber->SetMDTHitsPerChamber_IMO_Bin(dynamic_cast<TH2F*> (m_mdthitsperchamber_InnerMiddleOuterLumi[chamber->GetBarrelEndcapEnum()]));
      chamber->SetMDTHitsPerChamber_IMO_Bin(dynamic_cast<TH2F*> (m_mdthitsperchamber_InnerMiddleOuter_HighOcc[chamber->GetBarrelEndcapEnum()]));
      chamber->SetMDTHitsPerML_byLayer_Bins(dynamic_cast<TH2F*> (m_mdthitspermultilayerLumi[chamber->GetRegionEnum()][chamber->GetLayerEnum()])
          ,dynamic_cast<TH2F*> (m_mdthitsperML_byLayer[ (chamber->GetLayerEnum() < 3 ? chamber->GetLayerEnum() : 0) ]));
      if(m_doChamberHists){
        sc = bookMDTHistograms(chamber, *itr); 
      }
    }
    
  }//if newRunFlag()

  return sc; 
} 

/*----------------------------------------------------------------------------------*/
StatusCode MdtRawDataValAlg::fillHistograms()
/*----------------------------------------------------------------------------------*/ 
{  
  StatusCode sc = StatusCode::SUCCESS;

  //Set ATLASReadyFlag
  setIsATLASReady();

  m_numberOfEvents++;

  //Get lumiblock, event number, timing info
  sc = fillLumiBlock();
  if( sc.isFailure() ){
    ATH_MSG_ERROR( "Could Not Get LumiBlock Info" );
    return sc;
  }
  sc = GetEventNum();
  if( sc.isFailure() ){
    ATH_MSG_ERROR( "Could Not Get Event Num Info" );
    return sc;
  }
  sc= GetTimingInfo();
  if (sc.isFailure()){
       ATH_MSG_ERROR(" Cannot GetTimingInfo " );
       return sc;
  }
  ATH_MSG_DEBUG("MdtRawDataValAlg::MDT RawData Monitoring Histograms being filled" );

  //
  // Retrieve the LVL1 Muon RoIs:
  //
  StoreTriggerType(L1_UNKNOWN);  
  StringProperty lvl1_roi_key("LVL1MuonRoIs");

  const xAOD::MuonRoIContainer* muonRoIs; //const LVL1_ROI* lvl1_roi;

  sc = evtStore()->retrieve( muonRoIs, lvl1_roi_key);// lvl1_roi_key );
  if( sc.isFailure() ) {
    ATH_MSG_INFO( "Failed to access LVL1MuonRoIs in StoreGate with key: " << lvl1_roi_key );
    //     return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_VERBOSE( "Retrieved LVL1MuonRoIs object from StoreGate with key: " << lvl1_roi_key ); 
    xAOD::MuonRoIContainer::const_iterator mu_it = muonRoIs->begin(); 
    xAOD::MuonRoIContainer::const_iterator mu_it_end= muonRoIs->end();

    for( ; mu_it != mu_it_end; mu_it++){
      //ATH_MSG_ERROR( "(*mu_it)->getSource(): " << (*mu_it)->getSource() << ", is Muon_ROI::Endcap: " << ((*mu_it)->getSource()==(xAOD::MuonRoI::RoISource::Endcap)) << ", is Muon_ROI::Barrel: " << ((*mu_it)->getSource()==(xAOD::MuonRoI::RoISource::Barrel)) );
      if( (*mu_it)->getSource() == xAOD::MuonRoI::RoISource::Barrel) StoreTriggerType(L1_BARREL);
      if( (*mu_it)->getSource() == xAOD::MuonRoI::RoISource::Endcap ) StoreTriggerType(L1_ENDCAP);
    }
  }
  //   ATH_MSG_ERROR( "Stored m_trigtype: " << GetTriggerType() << " " << (GetTriggerType()==L1_UNKNOWN) << " " << (GetTriggerType()==L1_BARREL) << " " << (GetTriggerType()==L1_ENDCAP) );

  //declare MDT stuff 
  const Muon::MdtPrepDataContainer* mdt_container(0);

  sc = (*m_activeStore)->retrieve(mdt_container, m_key_mdt);
  if (sc.isFailure() || 0 == mdt_container ) 
  {
    ATH_MSG_ERROR(" Cannot retrieve MdtPrepDataContainer " << m_key_mdt );
    return sc;
  }
  ATH_MSG_DEBUG("****** mdtContainer->size() : " << mdt_container->size());  

  int nColl = 0;        // Number of MDT chambers with hits
  int nColl_ADCCut = 0; // Number of MDT chambers with hits above ADC cut
  int nPrd = 0;         // Total number of MDT prd digits
  int nPrdcut = 0;      // Total number of MDT prd digits with a cut on ADC>50.

  //declare RPC stuff
  const Muon::RpcPrepDataContainer* rpc_container;
  sc = (*m_activeStore)->retrieve(rpc_container, m_key_rpc);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( " Cannot retrieve RpcPrepDataContainer " << m_key_rpc );
    return sc;
  }


  ATH_MSG_DEBUG ("****** rpc->size() : " << rpc_container->size());

  Muon::RpcPrepDataContainer::const_iterator containerIt;

  /////////////////////////////this is the important plot!
  float Nhitsrpc = 0 ;
  for (containerIt = rpc_container->begin() ; containerIt != rpc_container->end() ; ++containerIt)
  {
    for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = (*containerIt)->begin(); rpcPrd!=(*containerIt)->end(); ++rpcPrd)
    {
      ++Nhitsrpc ;
    }}
  float Nhitsmdt = 0 ;
  bool isNoiseBurstCandidate = false;
  Muon::MdtPrepDataContainer::const_iterator MdtcontainerIt ;
  for (MdtcontainerIt = mdt_container->begin() ; MdtcontainerIt != mdt_container->end() ; ++MdtcontainerIt)
  {
    for (Muon::MdtPrepDataCollection::const_iterator mdtCollection=(*MdtcontainerIt)->begin(); mdtCollection!=(*MdtcontainerIt)->end(); ++mdtCollection )
    {
      ++Nhitsmdt ;
    }}
  if(Nhitsmdt > m_HighOccThreshold) isNoiseBurstCandidate = true;
  std::string type="MDT";
  std::string hardware_name;

  std::map<std::string,float> evnt_hitsperchamber_map;
  std::set<std::string>  chambers_from_tracks;
  
  if (m_doMdtESD==true) { 
    if(m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online) {

      const xAOD::MuonContainer* muons = evtStore()->retrieve< const xAOD::MuonContainer >("Muons");
      if (!muons) {
	ATH_MSG_ERROR ("Couldn't retrieve Muons container with key: Muons");
	return StatusCode::FAILURE;
      } 
      
      for(const auto mu : *muons){
	if(! (mu->muonType() == xAOD::Muon::Combined)) continue;
	xAOD::Muon::Quality quality = m_muonSelectionTool->getQuality(*mu);
	if (!(quality <= xAOD::Muon::Medium)) continue;
	const   xAOD::TrackParticle* tp = mu->primaryTrackParticle();
	if(tp){
	  const Trk::Track * trk= tp->track();
	  if(!trk){
	    continue;
	  }

	  uint8_t ntri_eta=0;
	  uint8_t n_phi=0; 
	  tp->summaryValue(ntri_eta, xAOD::numberOfTriggerEtaLayers); 
	  tp->summaryValue(n_phi, xAOD::numberOfPhiLayers); 
	  if(ntri_eta+n_phi==0) continue;
	  
          for (const Trk::MeasurementBase* hit : *trk->measurementsOnTrack()) {
	    const Trk::RIO_OnTrack* rot_from_track = dynamic_cast<const Trk::RIO_OnTrack*>(hit);
	    if(!rot_from_track) continue;
	    //              rot_from_track->dump(msg());
	    Identifier rotId = rot_from_track->identify();
	    if(!m_idHelper->isMdt(rotId)) continue;
	    IdentifierHash mdt_idHash;
	    MDTChamber* mdt_chamber = 0;
	    m_mdtIdHelper->get_module_hash( rotId, mdt_idHash );
	    sc = getChamber(mdt_idHash, mdt_chamber);
	    std::string mdt_chambername = mdt_chamber->getName();
	    chambers_from_tracks.insert(mdt_chambername);
	  }
	  
	}
      }

      //loop in MdtPrepDataContainer
      for (Muon::MdtPrepDataContainer::const_iterator containerIt = mdt_container->begin(); containerIt != mdt_container->end(); ++containerIt) {
        if (containerIt == mdt_container->end() || (*containerIt)->size()==0) continue;  //check if there are counts  
        nColl++;
        
        bool isHit_above_ADCCut = false;
        // loop over hits
        for (Muon::MdtPrepDataCollection::const_iterator mdtCollection=(*containerIt)->begin(); mdtCollection!=(*containerIt)->end(); ++mdtCollection ) 
        {
          nPrd++;
          hardware_name = getChamberName(*mdtCollection);
          float adc = (*mdtCollection)->adc();
          if(hardware_name.substr(0,3) == "BMG") adc /= 4.;
          if( adc > m_ADCCut ) 
          {
            nPrdcut++;
            isHit_above_ADCCut = true;
          }
          //      Identifier digcoll_id = (*mdtCollection)->identify();


          //Relic from cosmic days nolonger relevant      
          //      if (selectChambersRange(hardware_name, m_chamberName, m_mdtIdHelper->stationEta(digcoll_id), m_StationEta, m_mdtIdHelper->stationPhi(digcoll_id), m_StationPhi, m_StationSize) ) 
          //        {

          sc = fillMDTOverviewHistograms(*mdtCollection, isNoiseBurstCandidate);
          if(sc.isSuccess()) { 
            ATH_MSG_DEBUG("Filled MDTOverviewHistograms" );
          }
          else {
            ATH_MSG_ERROR("Failed to fill MDTOverviewHistograms" );
            return sc;
          }

	  //          sc = fillMDTSummaryHistograms(*mdtCollection, isNoiseBurstCandidate);
	  sc = fillMDTSummaryHistograms(*mdtCollection,  chambers_from_tracks, isNoiseBurstCandidate);
          if(sc.isSuccess())
          {
            ATH_MSG_DEBUG("Filled MDTSummaryHistograms " );
          }
          else {
            ATH_MSG_ERROR("Failed to Fill MDTSummaryHistograms" ); 
            return sc; 
          }

          if(m_doChamberHists){
            if(isATLASReady()) sc = fillMDTHistograms(*mdtCollection);
            if(sc.isSuccess()) 
            {
              ATH_MSG_DEBUG("Filled MDThistograms (chamber by chamber histos)" );
            }
            else {
              ATH_MSG_ERROR("Failed to Fill MDTHistograms" );
              return sc;
            }
          }       
          map<string,float>::iterator iter_hitsperchamber = evnt_hitsperchamber_map.find(hardware_name);
          if ( iter_hitsperchamber == evnt_hitsperchamber_map.end() ) { 
            evnt_hitsperchamber_map.insert( make_pair( hardware_name, 1 ) );
          } 
          else {
            iter_hitsperchamber->second += 1;
          }     

          
          if( adc >m_ADCCut) {
            map<string,float>::iterator iter_hitsperchamber = m_hitsperchamber_map.find(hardware_name);
            if ( iter_hitsperchamber == m_hitsperchamber_map.end() ) { 
              m_hitsperchamber_map.insert( make_pair( hardware_name, 1 ) );
            } 
            else {
              iter_hitsperchamber->second += 1;
            }                
          }       

          //        } //chamber name selection

        } // for loop over hits
        if( isHit_above_ADCCut ) 
          nColl_ADCCut++;
      } //loop in MdtPrepDataContainer
      int nHighOccChambers = 0;
      map<string,float>::iterator iterstat;

      for( iterstat = evnt_hitsperchamber_map.begin(); iterstat != evnt_hitsperchamber_map.end(); ++iterstat ) {
          map<string,float>::iterator iter_tubesperchamber = m_tubesperchamber_map.find(hardware_name);
          float nTubes = iter_tubesperchamber->second;
          float hits = iterstat->second;
          float occ = hits/nTubes;
          if ( occ > 0.1 ) nHighOccChambers++;
      }
      if (m_nummdtchamberswithHighOcc) m_nummdtchamberswithHighOcc->Fill(nHighOccChambers);
      else {ATH_MSG_DEBUG("m_nummdtchamberswithHighOcc not in hist list!" );}

      m_MdtNHitsvsRpcNHits->Fill(nPrd,Nhitsrpc);
      
      // TotalNumber_of_MDT_hits_per_event with RPCtrig
      /*if (mdtevents_RPCtrig){ 
        if(HasTrigBARREL()) mdtevents_RPCtrig->Fill(nPrdcut);
      }    
      else { 
        ATH_MSG_DEBUG("mdtevents_RPCtrig not in hist list!" );
      }


      // TotalNumber_of_MDT_hits_per_event with TGCtrig
      if (mdtevents_TGCtrig){ 
        if(HasTrigENDCAP()) mdtevents_TGCtrig->Fill(nPrdcut);
      }    
      else {
        ATH_MSG_DEBUG("mdtevents_TGCtrig not in hist list!" );
      }
*/
      // TotalNumber_of_MDT_hits_per_event with cut on ADC
      if (m_mdteventscutLumi) m_mdteventscutLumi->Fill(nPrdcut);    
      else {ATH_MSG_DEBUG("m_mdteventscutLumi not in hist list!" );}

      // TotalNumber_of_MDT_hits_per_event with cut on ADC (for high mult. evt)
      if (m_mdteventscutLumi_big) m_mdteventscutLumi_big->Fill(nPrdcut);    
      else {ATH_MSG_DEBUG("m_mdteventscutLumi_big not in hist list!" );}

      // TotalNumber_of_MDT_hits_per_event without cut on ADC
      if (m_mdteventsLumi) m_mdteventsLumi->Fill(nPrd);    
      else {ATH_MSG_DEBUG("m_mdteventsLumi not in hist list!" );}

      // TotalNumber_of_MDT_hits_per_event without cut on ADC (for high mult. evt)
      if (m_mdteventsLumi_big) m_mdteventsLumi_big->Fill(nPrd);    
      else {ATH_MSG_DEBUG("m_mdteventsLumi_big not in hist list!" );}

     /* if (m_numberOfEvents < 10000){    //only make this plot for first 10000 events 
        // TotalNumber_of_MDT_hits_vs_event_number with cut on ADC
        if (mdthitsvseventnumcut) mdthitsvseventnumcut->SetBinContent(m_numberOfEvents, nPrdcut);    
        else {ATH_MSG_DEBUG("mdthitsvseventnumcut not in hist list!" );}
      
        // TotalNumber_of_MDT_hits_vs_event_number without cut on ADC
        if (mdthitsvseventnum) mdthitsvseventnum->SetBinContent(m_numberOfEvents, nPrd);    
        else {ATH_MSG_DEBUG("mdthitsvseventnum not in hist list!" );}
      }*/

      //if(nPrdcut > 20000){
        //int realTime = m_time - m_firstTime;
        //std::cout << "printing out time... " << m_time << "and the time difference: " << realTime << std::endl;
        if (m_mdtglobalhitstime) m_mdtglobalhitstime->Fill(m_time - m_firstTime);
      //}

      // Number_of_Chambers_with_hits_per_event
      if (m_nummdtchamberswithhits) m_nummdtchamberswithhits->Fill(nColl);
      else {ATH_MSG_DEBUG("m_nummdtchamberswithhits not in hist list!" );}

      // Number_of_Chambers_with_hits_per_event_ADCCut
      if (m_nummdtchamberswithhits_ADCCut) m_nummdtchamberswithhits_ADCCut->Fill(nColl_ADCCut);
      else {ATH_MSG_DEBUG("m_nummdtchamberswithhits_ADCCut not in hist list!" );}

    }  //m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD   
  } //m_doMdtESD==true

  sc = evtStore()->retrieve(m_segms, m_segm_type);
  if(sc.isSuccess()) {
    ATH_MSG_DEBUG("Retrieved " << m_segm_type );
  }
  else {
    ATH_MSG_DEBUG("Couldn't retrieve " << m_segm_type );
    return sc;
  }

  if(isATLASReady()) sc=handleEvent_effCalc( m_segms); //, mdt_container );    
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Couldn't handleEvent_effCalc " );
    return sc;
  }

  return sc;
} 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdtRawDataValAlg::procHistograms(/*bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun */) {

  StatusCode sc = StatusCode::SUCCESS; 
  //if(isEndOfEventsBlock) {}
  if(endOfLumiBlockFlag()) {}
  //Replicate lowStat histograms to run directory if stable beam
  if( endOfLumiBlockFlag() && isATLASReady() && !m_isOnline ){
    //Book tdc adccut per region per lowStat
    sc = regHist((TH1F*) m_overalltdccut_segm_PR_Lumi[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalltdccut_segm_PR_Lumi[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalltdccut_segm_PR_Lumi[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalltdccut_segm_PR_Lumi[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);

    /*sc = regHist((TH1F*) m_overalltdcPR_HighOcc[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalltdcPR_HighOcc[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalltdcPR_HighOcc[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalltdcPR_HighOcc[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
    
    sc = regHist((TH1F*) m_overalltdcPR_HighOcc_ADCCut[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalltdcPR_HighOcc_ADCCut[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalltdcPR_HighOcc_ADCCut[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalltdcPR_HighOcc_ADCCut[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
    
    sc = regHist((TH1F*) m_overalladcPR_HighOcc[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalladcPR_HighOcc[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalladcPR_HighOcc[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalladcPR_HighOcc[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
    
    //Book tdcadc per region per lowStat, NoiseBurst cut
    sc = regHist((TH2F*) m_overalltdcadcPR_HighOcc[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);        
    sc = regHist((TH2F*) m_overalltdcadcPR_HighOcc[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);        
    sc = regHist((TH2F*) m_overalltdcadcPR_HighOcc[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);        
    sc = regHist((TH2F*) m_overalltdcadcPR_HighOcc[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);       */   
    
    //Book adc adccut per region on & off segment
    sc = regHist((TH1F*) m_overalladc_segm_PR_Lumi[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalladc_segm_PR_Lumi[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalladc_segm_PR_Lumi[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalladc_segm_PR_Lumi[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
    sc = regHist((TH1F*) m_overalladcPRLumi[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalladcPRLumi[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalladcPRLumi[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalladcPRLumi[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
    sc = regHist((TH1F*) m_overalladccutPRLumi[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalladccutPRLumi[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalladccutPRLumi[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalladccutPRLumi[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
  
    
    //TriggerAware TDC
    sc = regHist((TH1F*) m_overalltdccutPRLumi_RPCtrig[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi_RPCtrig[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi_RPCtrig[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi_RPCtrig[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi_TGCtrig[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi_TGCtrig[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi_TGCtrig[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);
    sc = regHist((TH1F*) m_overalltdccutPRLumi_TGCtrig[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);

    //Book tdcadc per region per lowStat
    sc = regHist((TH2F*) m_overalltdcadcPRLumi[enumBarrelA]->Clone(), m_mg->mongroup_brA_shift);        
    sc = regHist((TH2F*) m_overalltdcadcPRLumi[enumBarrelC]->Clone(), m_mg->mongroup_brC_shift);        
    sc = regHist((TH2F*) m_overalltdcadcPRLumi[enumEndCapA]->Clone(), m_mg->mongroup_ecA_shift);        
    sc = regHist((TH2F*) m_overalltdcadcPRLumi[enumEndCapC]->Clone(), m_mg->mongroup_ecC_shift);        
    
    //Book Global Hit Coverage/ML by Layer(Inner, Middle, Outer)
    sc = regHist((TH2F*) m_mdthitsperML_byLayer[enumInner]->Clone(), m_mg->mongroup_overview_shift);
    sc = regHist((TH2F*) m_mdthitsperML_byLayer[enumMiddle]->Clone(), m_mg->mongroup_overview_shift);
    sc = regHist((TH2F*) m_mdthitsperML_byLayer[enumOuter]->Clone(), m_mg->mongroup_overview_shift);

    for (int iecap=0;iecap<4;iecap++) {//BA,BC,EA,EC 
      for (int ilayer=0;ilayer<4;ilayer++) { //inner, middle, outer, extra
        /////////////////////////////////////////////////////////
        // Number of hits/effs per Multilayer 
        /////////////////////////////////////////////////////////
        if(iecap==enumBarrelA) sc = regHist((TH2F*)m_mdthitspermultilayerLumi[iecap][ilayer]->Clone(), m_mg->mongroup_brA_shift);
        else if(iecap==enumBarrelC) sc = regHist((TH2F*)m_mdthitspermultilayerLumi[iecap][ilayer]->Clone(), m_mg->mongroup_brC_shift);
        else if(iecap==enumEndCapA) sc = regHist((TH2F*)m_mdthitspermultilayerLumi[iecap][ilayer]->Clone(), m_mg->mongroup_ecA_shift);
        else sc = regHist((TH2F*)m_mdthitspermultilayerLumi[iecap][ilayer]->Clone(), m_mg->mongroup_ecC_shift);


        if(sc.isFailure()) {
          ATH_MSG_ERROR("mdthitspermultilayer Failed to register histogram" );       
          return sc;
        }
        ATH_MSG_DEBUG("Inside LowStat Loop, iecap="<< iecap << ", ilayer=" << ilayer );   


        /////////////////////////////////////////////////////////
        // Number of hits per chamber (all sectors in one plot)
        /////////////////////////////////////////////////////////
        if( ilayer==0 && ((iecap==0||iecap==2)) ) {
          ATH_MSG_DEBUG("Bookint mdthitsperchamber_InnerMiddleOuter");
          sc=regHist((TH2F*)m_mdthitsperchamber_InnerMiddleOuterLumi[iecap/2]->Clone(), m_mg->mongroup_overview_shift);
          sc=regHist((TH2F*)m_mdthitsperchamber_InnerMiddleOuter_HighOcc[iecap/2]->Clone(), m_mg->mongroup_overview_shift);

          ATH_MSG_DEBUG("Bookint m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi");
          sc=regHist((TH2F*)m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[iecap/2]->Clone(), m_mg->mongroup_overview_recoMon);

          if(sc.isFailure()) {
            ATH_MSG_ERROR("mdthitsperchamber_InnerMiddleOuter Failed to register histogram" );       
            return sc;
          }
        } // end if(ilayer==0&&(iecap==0||iecap==2) )
      }//layer
    }//ecap


    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdccut spectrum
    sc = regHist((TH1F*) m_overalltdccutLumi->Clone(), m_mg->mongroup_overview_shift);
    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdccut spectrum (along segms)
    sc = regHist((TH1F*) m_overalltdccut_segm_Lumi->Clone(), m_mg->mongroup_overview_shift);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall adc spectrum (along segms)
    sc = regHist((TH1F*) m_overalladc_segm_Lumi->Clone(), m_mg->mongroup_overview_shift);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall adc spectrum
    sc = regHist((TH1F*) m_overalladc_Lumi->Clone(), m_mg->mongroup_overview_shift);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event with a cut on ADC 
    sc = regHist((TH1F*) m_mdteventscutLumi->Clone(), m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event with a cut on ADC  (for high mult. events)
    sc = regHist((TH1F*) m_mdteventscutLumi_big->Clone(), m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event without a cut on ADC 
    sc = regHist((TH1F*) m_mdteventsLumi->Clone(), m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event without a cut on ADC  (for high mult. events)
    sc = regHist((TH1F*) m_mdteventsLumi_big->Clone(), m_mg->mongroup_overview_shift);
  


    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event_RPCtrig 
    //sc = regHist((TH1F*) mdtevents_RPCtrig->Clone(), m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event_TGCtrig 
   // sc = regHist((TH1F*) mdtevents_TGCtrig->Clone(), m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdc vs adc spectrum 
    sc = regHist((TH2F*)m_overalltdcadcLumi->Clone(), m_mg->mongroup_overview_shift);        
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdccut RPCtrig spectrum 
    //sc = regHist((TH1F*) m_overalltdccut_RPCtrig->Clone(), m_mg->mongroup_overview_shift);        

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdccut TGCtrig spectrum 
   // sc = regHist((TH1F*) m_overalltdccut_TGCtrig->Clone(), m_mg->mongroup_overview_shift);        
    
    //histo path for m_MdtNHitsvsRpcNHits
    //sc = regHist((TH2F*) m_MdtNHitsvsRpcNHits->Clone(), m_mg->mongroup_overview_shift);

    /*////////////////////////////////////////////////////////////////////////////////////// 
    //histo paths for noise burst monitoring!
    sc = regHist((TH1F*) m_overalltdcHighOcc->Clone(), m_mg->mongroup_overview_shift);    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo paths for noise burst monitoring!
    sc = regHist((TH1F*) m_overalltdcHighOcc_ADCCut->Clone(), m_mg->mongroup_overview_shift);    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo paths for noise burst monitoring!
    sc = regHist((TH1F*) m_overalladc_HighOcc->Clone(), m_mg->mongroup_overview_shift);    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdc vs adc spectrum 
    sc = regHist((TH2F*)m_overalltdcadcHighOcc->Clone(), m_mg->mongroup_overview_shift);        */
  }



  if(endOfRunFlag()) {

    ATH_MSG_DEBUG("********Reached Last Event in MdtRawDataValAlg !!!" );   
    ATH_MSG_DEBUG("MdtRawDataValAlg finalize()" );

    if(m_mdtchamberstat){
      m_mdtchamberstat->SetStats(0);
      //m_mdtchamberstat->SetBit(TH1::kCanRebin);
      m_mdtchamberstat->LabelsDeflate("X");
    }
    map<string,float>::iterator iterstat;
    char c[3]="  ";
    for( iterstat = m_hitsperchamber_map.begin(); iterstat != m_hitsperchamber_map.end(); ++iterstat ) {
      const char* chambername_char = iterstat->first.c_str();
      float hits = iterstat->second;
      if(m_mdtchamberstat)
        m_mdtchamberstat->Fill(chambername_char,hits);

      /**Fills counts per hardware chambers in phi slice  plot*/
      //Look for chamber names like XXXXXNMYYYYY where NM is a number in 01...16. And do it efficiently:
      if (iterstat->first.length()<7)
        continue;
      c[0]=chambername_char[5];
      c[1]=chambername_char[6];
      if (!(c[0]=='0'||c[0]=='1')||c[1]<'0'||c[1]>'9')
        continue;
      int i(atoi(c)-1);
      if (i<0||i>15)
        continue;
      //Now fill the histogram
      TH1* h = m_mdtchamberstatphislice[i];
      if (h)
        h->Fill(chambername_char,hits);
    }// for in m_hitsperchamber_map

    if(m_mdtchamberstat)
      m_mdtchamberstat->LabelsDeflate("X");

    //Deflate phislice m_mdtchamberstatphislice
    for(int j=0; j<=15; ++j) {
      if( m_mdtchamberstatphislice[j] ) m_mdtchamberstatphislice[j]->LabelsDeflate("X");
    }

  } // endOfRunFlag()

  return sc;
}  

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//input hardware name as produced from convertChamberName()
StatusCode MdtRawDataValAlg::bookMDTHistograms( MDTChamber* chamber, Identifier digcoll_id ) {
  StatusCode sc = StatusCode::SUCCESS;

  std::string hardware_name = chamber->getName();
  IdentifierHash idHash;
  m_mdtIdHelper->get_module_hash(digcoll_id, idHash);

  int tubeIdMax = GetTubeMax(digcoll_id, hardware_name);

  //declare a group of histograms
  std::string subdir_path = hardware_name.substr(0,1) + hardware_name.substr(4,1);
  if ( subdir_path.at(1) == 'B' ) subdir_path.at(1) = 'A'; //Place BOG0B12,14 in MDTBA

  MonGroup* mongroup_chambers_expert = 0;
  if( subdir_path=="BA" ) mongroup_chambers_expert = &(m_mg->mongroup_chambers_expert_MDTBA);
  else if( subdir_path=="BC" ) mongroup_chambers_expert = &(m_mg->mongroup_chambers_expert_MDTBC);
  else if( subdir_path=="EA" ) mongroup_chambers_expert = &(m_mg->mongroup_chambers_expert_MDTEA);
  else mongroup_chambers_expert = &(m_mg->mongroup_chambers_expert_MDTEC);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt EfficiencyEntries
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdt_effEntries) 
    sc = bookMDTHisto_chambers(chamber->mdt_effEntries, hardware_name + "_MDT_Station_EFFENTRIES", "tubeID", "Number of Entries",
        tubeIdMax, 1, tubeIdMax+1, *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt EfficiencyCounts
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdt_effCounts) 
    sc = bookMDTHisto_chambers(chamber->mdt_effCounts, hardware_name + "_MDT_Station_EFFCOUNTS", "tubeID", "Number of Counts",
        tubeIdMax, 1, tubeIdMax+1, *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt EfficiencyPerTube
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdt_effPerTube) 
    sc = bookMDTHisto_chambers(chamber->mdt_effPerTube, hardware_name + "_MDT_Station_EFFPERTUBE", "tubeID", "Efficiency",
        tubeIdMax, 1, tubeIdMax+1, *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tdc 
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdttdc) 
    sc = bookMDTHisto_chambers(chamber->mdttdc, hardware_name + "_MDT_Station_TDC", "[nsec]", "Number of Entries",
        100, 0, 2000.,  *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tdc trigger type RPC ML1 ADC Cut
  //////////////////////////////////////////////////////////////////////////////////////   
 /* if(m_do_mdttdccut_RPCtrig_ML1) 
    sc = bookMDTHisto_chambers(chamber->mdttdccut_RPCtrig_ML1, hardware_name + "_MDT_Station_TDC_ADCCut_RPCtrig_ML1", "[nsec]", "Number of Entries",
        100, 0, 2000.,  *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tdc trigger type TGC ML1
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdttdccut_TGCtrig_ML1) 
    sc = bookMDTHisto_chambers(chamber->mdttdccut_TGCtrig_ML1, hardware_name + "_MDT_Station_TDC_ADCCut_TGCtrig_ML1", "[nsec]", "Number of Entries",
        100, 0, 2000.,  *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tdc trigger type RPC ML2 ADC Cut
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdttdccut_RPCtrig_ML2) 
    sc = bookMDTHisto_chambers(chamber->mdttdccut_RPCtrig_ML2, hardware_name + "_MDT_Station_TDC_ADCCut_RPCtrig_ML2", "[nsec]", "Number of Entries",
        100, 0, 2000.,  *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tdc trigger type TGC ML2
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdttdccut_TGCtrig_ML2) 
    sc = bookMDTHisto_chambers(chamber->mdttdccut_TGCtrig_ML2, hardware_name + "_MDT_Station_TDC_ADCCut_TGCtrig_ML2", "[nsec]", "Number of Entries",
        100, 0, 2000.,  *mongroup_chambers_expert);
*/
  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tdccut ML1
  ////////////////////////////////////////////////////////////////////////////////////// 
  if(m_do_mdttdccut_ML1) 
    sc = bookMDTHisto_chambers(chamber->mdttdccut_ML1, hardware_name + "_MDT_Station_TDC_ML1_ADCCut", "[nsec]", "Number of Entries",
        100, 0, 2000.,  *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tdccut ML2
  ////////////////////////////////////////////////////////////////////////////////////// 
  if(m_do_mdttdccut_ML2) 
    sc = bookMDTHisto_chambers(chamber->mdttdccut_ML2, hardware_name + "_MDT_Station_TDC_ML2_ADCCut", "[nsec]", "Number of Entries",
        100, 0, 2000.,  *mongroup_chambers_expert);
  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt adc on segment ML1
  ////////////////////////////////////////////////////////////////////////////////////// 
  if(m_do_mdtadc_onSegm_ML1) 
    sc = bookMDTHisto_chambers(chamber->mdtadc_onSegm_ML1, hardware_name + "_MDT_Station_ADC_onSegm_ML1", "[adc counts]", "Number of Entries",
        100, 0, 400., *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt adc on segment ML2
  ////////////////////////////////////////////////////////////////////////////////////// 
  if(m_do_mdtadc_onSegm_ML2) 
    sc = bookMDTHisto_chambers(chamber->mdtadc_onSegm_ML2, hardware_name + "_MDT_Station_ADC_onSegm_ML2", "[adc counts]", "Number of Entries",
        100, 0, 400., *mongroup_chambers_expert);
  
  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt adc 
  //////////////////////////////////////////////////////////////////////////////////////   
  if(m_do_mdtadc) 
    sc = bookMDTHisto_chambers(chamber->mdtadc, hardware_name + "_MDT_Station_ADC", "[adc counts]", "Number of Entries",
        100, 0, 400., *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt multilayer 
/*  if(m_do_mdtmultil) 
    sc = bookMDTHisto_chambers(chamber->mdtmultil, hardware_name + "_MDT_Station_MULTIL_ADCCut", "multilayerID", "Number of Entries",
        4, 0, 4., *mongroup_chambers_expert);
*/
  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt layer 
  if(m_do_mdtlayer) 
    sc = bookMDTHisto_chambers(chamber->mdtlayer, hardware_name + "_MDT_Station_LAYER_ADCCut", "layerID", "Number of Entries",
        10, 0, 10., *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tube 
  if(m_do_mdttube) 
    sc = bookMDTHisto_chambers(chamber->mdttube, hardware_name + "_MDT_Station_TUBE_ADCCut", "tubeID", "Number of Entries",
        tubeIdMax, 1, tubeIdMax+1, *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tube bkgrd hits
  /*if(m_do_mdttube_bkgrd) 
    sc = bookMDTHisto_chambers(chamber->mdttube_bkgrd, hardware_name + "_MDT_Station_TUBE_BKGRD_ADCCut", "tubeID", "Total Number of Hits",
        tubeIdMax, 1, tubeIdMax+1, *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tube counts for offline noise calculation
  if(m_do_mdttube_fornoise) 
    sc = bookMDTHisto_chambers(chamber->mdttube_fornoise, hardware_name + "_MDT_Station_TUBE_fornoise", "tubeID", "Number of Entries",
        tubeIdMax, 1, tubeIdMax+1, *mongroup_chambers_expert);
*/
  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt tube masked info
  if(m_do_mdttube_masked) {
    sc = bookMDTHisto_chambers(chamber->mdttube_masked, hardware_name + "_MDT_Station_TUBE_masked", "tubeID", "Number of Entries",
        tubeIdMax, 1, tubeIdMax+1, *mongroup_chambers_expert);    
    sc = fillMDTMaskedTubes(idHash, chamber->getName(), chamber->mdttube_masked);
  }

  ////////////////////////////////////////////////////////////////////////////////////// 
  //histo path for mdt mezzannine 
  if(m_do_mdtmezz) 
    sc = bookMDTHisto_chambers(chamber->mdtmezz, hardware_name + "_MDT_Station_MEZZ_ADCCut", "mezzID",
        "Number of Entries", 20, 0, 20., *mongroup_chambers_expert);

  ////////////////////////////////////////////////////////////////////////////////////// 
  //2D hists
  if(m_chamber_2D) {
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt tdcadc 
    if(m_do_mdttdcadc) 
      sc = bookMDTHisto_chambers_2D(chamber->mdttdcadc, hardware_name + "_MDT_Station_TDCADC", "[nsec]", "[adc counts]",
          50, 0, 2000., 20. ,0., 400., *mongroup_chambers_expert);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt drift radius vs drift time
    if(m_do_mdt_DRvsDT) 
      sc = bookMDTHisto_chambers_2D(chamber->mdt_DRvsDT, hardware_name + "_MDT_Station_DRvsDT", "Drift Radius [mm]", "Drift Time [nsec]",
          80, -20, 20., 120, -200., 1000., *mongroup_chambers_expert);


    //     ////////////////////////////////////////////////////////////////////////////////////// 
    //     //histo path for mdt drift radius vs drift time
    if(m_do_mdt_DRvsDRerr) 
      sc = bookMDTHisto_chambers_2D(chamber->mdt_DRvsDRerr, hardware_name + "_MDT_Station_DRvsDRerr", "Drift Radius [mm]", "Error [mm]",
          80, -20, 20., 80, -5., 5., *mongroup_chambers_expert);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt drift radius vs drift time
    if(m_do_mdt_DRvsSegD) 
      sc = bookMDTHisto_chambers_2D(chamber->mdt_DRvsSegD, hardware_name + "_MDT_Station_DRvsSegD", "Drift Radius [mm]",
          "Segm dist. to tube [mm]", 80, -20, 20., 80, -20., 20., *mongroup_chambers_expert);
  }

  ATH_MSG_DEBUG("Booked bookMDTHistograms successfully" );  

  return sc;
}

StatusCode MdtRawDataValAlg::bookMDTSummaryHistograms(/* bool isNewEventsBlock, */bool newLumiBlock, bool newRun) {
  StatusCode sc = StatusCode::SUCCESS;
  //if( isNewEventsBlock ){}
  if( newLumiBlock ){}
  //book these histos as lowStat interval if not online monitoring
  if( (newLowStatIntervalFlag() && !m_isOnline) || (m_isOnline && newRun) ){
    //Book tdc adccut per region per lowStat
    sc = bookMDTHisto_overview(m_overalltdccut_segm_PR_Lumi[enumBarrelA], "MDTTDC_segm_Summary_ADCCut_BA", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_brA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccut_segm_PR_Lumi[enumBarrelC], "MDTTDC_segm_Summary_ADCCut_BC", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_brC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccut_segm_PR_Lumi[enumEndCapA], "MDTTDC_segm_Summary_ADCCut_EA", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_ecA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccut_segm_PR_Lumi[enumEndCapC], "MDTTDC_segm_Summary_ADCCut_EC", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_ecC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi[enumBarrelA], "MDTTDC_Summary_ADCCut_BA", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_brA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi[enumBarrelC], "MDTTDC_Summary_ADCCut_BC", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_brC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi[enumEndCapA], "MDTTDC_Summary_ADCCut_EA", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_ecA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi[enumEndCapC], "MDTTDC_Summary_ADCCut_EC", "[nsec]", "Number of Entries",
        120, 0., 2000.,m_mg->mongroup_ecC_shiftLumi);
    
    //Book adc adccut per region on & off segment
    sc = bookMDTHisto_overview(m_overalladc_segm_PR_Lumi[enumBarrelA], "MDTADC_segm_Summary_BA", "[adc counts]", "Number of Entries",
                                100, 0.5, 400.5,m_mg->mongroup_brA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladc_segm_PR_Lumi[enumBarrelC], "MDTADC_segm_Summary_BC", "[adc counts]", "Number of Entries",
                                100, 0.5, 400.5,m_mg->mongroup_brC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladc_segm_PR_Lumi[enumEndCapA], "MDTADC_segm_Summary_EA", "[adc counts]", "Number of Entries",
                                100, 0.5, 400.5,m_mg->mongroup_ecA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladc_segm_PR_Lumi[enumEndCapC], "MDTADC_segm_Summary_EC", "[adc counts]", "Number of Entries",
                                100, 0.5, 400.5,m_mg->mongroup_ecC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladcPRLumi[enumBarrelA], "MDTADC_Summary_BA", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_brA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladcPRLumi[enumBarrelC], "MDTADC_Summary_BC", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_brC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladcPRLumi[enumEndCapA], "MDTADC_Summary_EA", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_ecA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladcPRLumi[enumEndCapC], "MDTADC_Summary_EC", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_ecC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladccutPRLumi[enumBarrelA], "MDTADC_Summary_ADCCut_BA", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_brA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladccutPRLumi[enumBarrelC], "MDTADC_Summary_ADCCut_BC", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_brC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladccutPRLumi[enumEndCapA], "MDTADC_Summary_ADCCut_EA", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_ecA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalladccutPRLumi[enumEndCapC], "MDTADC_Summary_ADCCut_EC", "[adc counts]", "Number of Entries",
                                100, 0., 400.,m_mg->mongroup_ecC_shiftLumi);

    //TriggerAware TDC
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_RPCtrig[enumBarrelA], "MDTTDC_Summary_ADCCut_BA_RPC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_brA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_RPCtrig[enumBarrelC], "MDTTDC_Summary_ADCCut_BC_RPC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_brC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_RPCtrig[enumEndCapA], "MDTTDC_Summary_ADCCut_EA_RPC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_ecA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_RPCtrig[enumEndCapC], "MDTTDC_Summary_ADCCut_EC_RPC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_ecC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_TGCtrig[enumBarrelA], "MDTTDC_Summary_ADCCut_BA_TGC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_brA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_TGCtrig[enumBarrelC], "MDTTDC_Summary_ADCCut_BC_TGC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_brC_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_TGCtrig[enumEndCapA], "MDTTDC_Summary_ADCCut_EA_TGC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_ecA_shiftLumi);
    sc = bookMDTHisto_overview(m_overalltdccutPRLumi_TGCtrig[enumEndCapC], "MDTTDC_Summary_ADCCut_EC_TGC", "[nsec]", 
        "Number of Entries", 120, 0., 2000.,m_mg->mongroup_ecC_shiftLumi);

    //Book tdcadc per region per lowStat
    sc = bookMDTHisto_overview_2D(m_overalltdcadcPRLumi[enumBarrelA], "Overall_TDCADC_spectrum_BA", "[nsec]", "[adc counts]", 50, 0, 2000.,
        20. ,0., 400., m_mg->mongroup_brA_shiftLumi);        
    sc = bookMDTHisto_overview_2D(m_overalltdcadcPRLumi[enumBarrelC], "Overall_TDCADC_spectrum_BC", "[nsec]", "[adc counts]", 50, 0, 2000.,
        20. ,0., 400., m_mg->mongroup_brC_shiftLumi);        
    sc = bookMDTHisto_overview_2D(m_overalltdcadcPRLumi[enumEndCapA], "Overall_TDCADC_spectrum_EA", "[nsec]", "[adc counts]", 50, 0, 2000.,
        20. ,0., 400., m_mg->mongroup_ecA_shiftLumi);        
    sc = bookMDTHisto_overview_2D(m_overalltdcadcPRLumi[enumEndCapC], "Overall_TDCADC_spectrum_EC", "[nsec]", "[adc counts]", 50, 0, 2000.,
        20. ,0., 400., m_mg->mongroup_ecC_shiftLumi);        

    
    //Book Global Hit Coverage/ML by Layer(Inner, Middle, Outer)
    sc = bookMDTHisto_overview_2D(m_mdthitsperML_byLayer[enumInner], "NumberOfHitsInMDTInner_ADCCut","#eta station","#phi station", 
        1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
    sc = bookMDTHisto_overview_2D(m_mdthitsperML_byLayer[enumMiddle], "NumberOfHitsInMDTMiddle_ADCCut","#eta station","#phi station", 
        1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
    sc = bookMDTHisto_overview_2D(m_mdthitsperML_byLayer[enumOuter], "NumberOfHitsInMDTOuter_ADCCut","#eta station","#phi station", 
        1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
    sc = binMdtGlobal_byLayer(m_mdthitsperML_byLayer[enumInner], m_mdthitsperML_byLayer[enumMiddle], m_mdthitsperML_byLayer[enumOuter]);
    //histo path for MDT Summary plots Barrel-EndCap 
    std::string ecap[4]={"BA","BC","EA","EC"};
    std::string layer[4]={"Inner","Middle","Outer","Extra"};
    std::string MDTHits_BE[2] = {"Barrel", "EndCap"};

    for (int iecap=0;iecap<4;iecap++) {//BA,BC,EA,EC 
      for (int ilayer=0;ilayer<4;ilayer++) { //inner, middle, outer, extra
        /////////////////////////////////////////////////////////
        // Number of hits/effs per Multilayer 
        /////////////////////////////////////////////////////////
        if(iecap==enumBarrelA) sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
            "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
            "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_brA_shiftLumi);
        else if(iecap==enumBarrelC) sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
            "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
            "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_brC_shiftLumi);
        else if(iecap==enumEndCapA) sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
            "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
            "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_ecA_shiftLumi);
        else sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
            "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
            "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_ecC_shiftLumi);

        string xAxis = ecap[iecap].substr(0,1) + layer[ilayer].substr(0,1) + ecap[iecap].substr(1,1);
        sc=binMdtRegional(m_mdthitspermultilayerLumi[iecap][ilayer], xAxis);

        if(sc.isFailure()) {
          ATH_MSG_ERROR("mdthitspermultilayer Failed to register histogram lowStat" );       
          return sc;
        }
        ATH_MSG_DEBUG("Inside LowStat Loop, iecap="<< iecap << ", ilayer=" << ilayer );   

        /////////////////////////////////////////////////////////
        // Number of hits per chamber (all sectors in one plot)
        /////////////////////////////////////////////////////////
        if( ilayer==0 && ((iecap==0||iecap==2)) ) {
          ATH_MSG_DEBUG("Bookint mdthitsperchamber_InnerMiddleOuter LowStat");
          sc=bookMDTHisto_overview_2D(m_mdthitsperchamber_InnerMiddleOuterLumi[iecap/2], "NumberOfHitsIn"+MDTHits_BE[iecap/2]+"PerChamber_ADCCut",
              "[Eta]", "[Layer,Phi]", 1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
          sc=binMdtGlobal(m_mdthitsperchamber_InnerMiddleOuterLumi[iecap/2], MDTHits_BE[iecap/2].at(0) );
          
          sc=bookMDTHisto_overview_2D(m_mdthitsperchamber_InnerMiddleOuter_HighOcc[iecap/2], "NumberOfHitsIn"+MDTHits_BE[iecap/2]+"PerChamber_ADCCut_NoiseBurst",
              "[Eta]", "[Layer,Phi]", 1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
          sc=binMdtGlobal(m_mdthitsperchamber_InnerMiddleOuter_HighOcc[iecap/2], MDTHits_BE[iecap/2].at(0) );
          
          ATH_MSG_DEBUG("Bookint m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi LowStat");
          sc=bookMDTHisto_overview_2D(m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[iecap/2], "NumberOfHitsIn"+MDTHits_BE[iecap/2]+"PerChamber_onSegm_ADCCut",
              "[Eta]", "[Layer,Phi]", 1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi_recoMon);
          sc=binMdtGlobal(m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[iecap/2], MDTHits_BE[iecap/2].at(0) );

          if(sc.isFailure()) {
            ATH_MSG_ERROR("mdthitsperchamber_InnerMiddleOuter Failed to register histogram in lowStat" );       
            return sc;
          }
        } // end if(ilayer==0&&(iecap==0||iecap==2) )
      }//layer
    }//ecap
  }//newLowStatIntervalFlag()

  if(newRun){
    //     //Book t0 tmax tdrift summary plots
    //     //Just create these in the post-processing    
    //     sc = bookMDTHisto_overview(mdttdccut_t0[enumBarrelA], "MDT_t0_BA", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_brA_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_t0[enumBarrelC], "MDT_t0_BC", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_brC_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_t0[enumEndCapA], "MDT_t0_EA", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_ecA_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_t0[enumEndCapC], "MDT_t0_EC", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_ecC_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tmax[enumBarrelA], "MDT_tmax_BA", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_brA_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tmax[enumBarrelC], "MDT_tmax_BC", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_brC_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tmax[enumEndCapA], "MDT_tmax_EA", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_ecA_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tmax[enumEndCapC], "MDT_tmax_EC", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_ecC_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tdrift[enumBarrelA], "MDT_tdrift_BA", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_brA_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tdrift[enumBarrelC], "MDT_tdrift_BC", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_brC_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tdrift[enumEndCapA], "MDT_tdrift_EA", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_ecA_shift);
    //     sc = bookMDTHisto_overview(mdttdccut_tdrift[enumEndCapC], "MDT_tdrift_EC", "sector", "[nsec]", 1, 0, 1,m_mg->mongroup_ecC_shift);

    //histo path for MDT Summary plots Barrel-EndCap 
    std::string ecap[4]={"BA","BC","EA","EC"};
    std::string layer[4]={"Inner","Middle","Outer","Extra"};
    std::string MDTHits_BE[2] = {"Barrel", "EndCap"};
    std::string crate[4]={"01","02","03","04"};

    for (int iecap=0;iecap<4;iecap++) {//BA,BC,EA,EC 
      for (int ilayer=0;ilayer<4;ilayer++) { //inner, middle, outer, extra
        /////////////////////////////////////////////////////////
        // Number of hits/effs per Multilayer 
        /////////////////////////////////////////////////////////
        //To-do Remove these from ESD monitoring in place of Post-Processing
        if(iecap==enumBarrelA){
          sc = bookMDTHisto_overview_2D(m_mdteffpermultilayer[iecap][ilayer], "effsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut", 
              "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_brA_shift);
        }
        else if(iecap==enumBarrelC){
          sc = bookMDTHisto_overview_2D(m_mdteffpermultilayer[iecap][ilayer], "effsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut", "[Eta]", 
              "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_brC_shift);
        }
        else if(iecap==enumEndCapA){
          sc = bookMDTHisto_overview_2D(m_mdteffpermultilayer[iecap][ilayer], "effsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut", "[Eta]", 
              "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_ecA_shift);
        }
        else {
          sc = bookMDTHisto_overview_2D(m_mdteffpermultilayer[iecap][ilayer], "effsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut", "[Eta]",
              "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_ecC_shift);
        }

        std::string xAxis = ecap[iecap].substr(0,1) + layer[ilayer].substr(0,1) + ecap[iecap].substr(1,1);
        sc=binMdtRegional(m_mdteffpermultilayer[iecap][ilayer], xAxis);

        if(sc.isFailure()) {
          ATH_MSG_ERROR("mdthitspermultilayer Failed to register histogram " );       
          return sc;
        }
        ATH_MSG_DEBUG("Inside Loop, iecap="<< iecap << ", ilayer=" << ilayer );   

        /////////////////////////////////////////////////////////
        // Number of hits per chamber (all sectors in one plot)
        /////////////////////////////////////////////////////////
        if( ilayer==0 && ((iecap==0||iecap==2)) ) {
          ATH_MSG_DEBUG("Bookint m_mdteffperchamber_InnerMiddleOuter");
          sc=bookMDTHisto_overview_2D(m_mdteffperchamber_InnerMiddleOuter[iecap/2], "effsIn"+MDTHits_BE[iecap/2]+"PerChamber_ADCCut", 
              "[Eta]", "[Layer,Phi]", 1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shift);

          sc=binMdtGlobal(m_mdteffperchamber_InnerMiddleOuter[iecap/2], MDTHits_BE[iecap/2].at(0) );

          if(sc.isFailure()) {
            ATH_MSG_ERROR("mdthitsperchamber_InnerMiddleOuter Failed to register histogram " );       
            return sc;
          }
        } // end if(ilayer==0&&(iecap==0||iecap==2) )


        //Book adc, tdc histograms for noise burst monitoring
        sc = bookMDTHisto_overview(m_overalladcPR_HighOcc[enumBarrelA], "MDTADC_NoiseBursts_BA", "[adc counts]", "Number of Entries",
                                    100, 0., 400.,m_mg->mongroup_brA_shift);
        sc = bookMDTHisto_overview(m_overalladcPR_HighOcc[enumBarrelC], "MDTADC_NoiseBursts_BC", "[adc counts]", "Number of Entries",
                                    100, 0., 400.,m_mg->mongroup_brC_shift);
        sc = bookMDTHisto_overview(m_overalladcPR_HighOcc[enumEndCapA], "MDTADC_NoiseBursts_EA", "[adc counts]", "Number of Entries",
                                    100, 0., 400.,m_mg->mongroup_ecA_shift);
        sc = bookMDTHisto_overview(m_overalladcPR_HighOcc[enumEndCapC], "MDTADC_NoiseBursts_EC", "[adc counts]", "Number of Entries",
                                    100, 0., 400.,m_mg->mongroup_ecC_shift);
        sc = bookMDTHisto_overview_2D(m_overalltdcadcPR_HighOcc[enumBarrelA], "Overall_TDCADC_spectrum_BA_NoiseBursts", "[nsec]", "[adc counts]", 50, 0, 2000.,
            40, 0., 400., m_mg->mongroup_brA_shift);        
        sc = bookMDTHisto_overview_2D(m_overalltdcadcPR_HighOcc[enumBarrelC], "Overall_TDCADC_spectrum_BC_NoiseBursts", "[nsec]", "[adc counts]", 50, 0, 2000.,
            40, 0., 400., m_mg->mongroup_brC_shift);        
        sc = bookMDTHisto_overview_2D(m_overalltdcadcPR_HighOcc[enumEndCapA], "Overall_TDCADC_spectrum_EA_NoiseBursts", "[nsec]", "[adc counts]", 50, 0, 2000.,
            40, 0., 400., m_mg->mongroup_ecA_shift);        
        sc = bookMDTHisto_overview_2D(m_overalltdcadcPR_HighOcc[enumEndCapC], "Overall_TDCADC_spectrum_EC_NoiseBursts", "[nsec]", "[adc counts]", 50, 0, 2000.,
            40, 0., 400., m_mg->mongroup_ecC_shift);       
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc[enumBarrelA], "MDTTDC_NoiseBursts_BA", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_brA_shift);
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc[enumBarrelC], "MDTTDC_NoiseBursts_BC", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_brC_shift);
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc[enumEndCapA], "MDTTDC_NoiseBursts_EA", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_ecA_shift);
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc[enumEndCapC], "MDTTDC_NoiseBursts_EC", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_ecC_shift);
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc_ADCCut[enumBarrelA], "MDTTDC_NoiseBursts_ADCCut_BA", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_brA_shift);
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc_ADCCut[enumBarrelC], "MDTTDC_NoiseBursts_ADCCut_BC", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_brC_shift);
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc_ADCCut[enumEndCapA], "MDTTDC_NoiseBursts_ADCCut_EA", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_ecA_shift);
        sc = bookMDTHisto_overview(m_overalltdcPR_HighOcc_ADCCut[enumEndCapC], "MDTTDC_NoiseBursts_ADCCut_EC", "[nsec]", "Number of Entries",
            120, 0., 2000.,m_mg->mongroup_ecC_shift);
        /////////////////////////////////////////////////
        // Book occupancy as function of lumiblock
        /////////////////////////////////////////////////

        if(ilayer != 3){ // put extras in outer
          std::string lbhisttitle;
          if(ilayer == 2) lbhisttitle = "OccupancyVsLB_"+ecap[iecap]+layer[ilayer]+"PlusExtra";
          else lbhisttitle = "OccupancyVsLB_"+ecap[iecap]+layer[ilayer];
          if(iecap==enumBarrelA){
            sc = bookMDTHisto_OccVsLB(m_mdtoccvslb[iecap][ilayer],lbhisttitle,"LB","[Eta,Phi]",834,1,2502,100,1,100,m_mg->mongroup_brA_shift);
            if(sc.isFailure()) {
              ATH_MSG_ERROR("m_mdtoccvslb Failed to register histogram " );       
              return sc;
            }
          }
          else if(iecap==enumBarrelC){
            sc = bookMDTHisto_OccVsLB(m_mdtoccvslb[iecap][ilayer],lbhisttitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_brC_shift);
            if(sc.isFailure()) {
              ATH_MSG_ERROR("m_mdtoccvslb Failed to register histogram " );       
              return sc;
            }
          }
          else if(iecap==enumEndCapA){
            sc = bookMDTHisto_OccVsLB(m_mdtoccvslb[iecap][ilayer],lbhisttitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_ecA_shift);
            if(sc.isFailure()) {
              ATH_MSG_ERROR("m_mdtoccvslb Failed to register histogram " );       
              return sc;
            }
          }
          else{
            sc = bookMDTHisto_OccVsLB(m_mdtoccvslb[iecap][ilayer],lbhisttitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_ecC_shift);
            if(sc.isFailure()) {
              ATH_MSG_ERROR("m_mdtoccvslb Failed to register histogram " );       
              return sc;
            }
          }

          sc = binMdtOccVsLB(m_mdtoccvslb[iecap][ilayer],iecap,ilayer);
          if(sc.isFailure()) {
            ATH_MSG_ERROR("m_mdtoccvslb Failed to register histogram " );       
            return sc;
          }

        }

        /////////////////////////////////////////////////
        // Book occupancy as function of lumiblock by crate
        /////////////////////////////////////////////////

        std::string lbCrate_histtitle = "OccupancyVsLB_"+ecap[iecap]+crate[ilayer];
        if(iecap==enumBarrelA){
          sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_by_crate[iecap][ilayer],lbCrate_histtitle,"LB","[Eta,Phi]",834,1,2502,100,1,100,m_mg->mongroup_brA_shift);
          if(sc.isFailure()) {
            ATH_MSG_ERROR("m_mdtoccvslb_by_crate Failed to register histogram " );       
            return sc;
          }
        }
        else if(iecap==enumBarrelC){
          sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_by_crate[iecap][ilayer],lbCrate_histtitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_brC_shift);
          if(sc.isFailure()) {
            ATH_MSG_ERROR("m_mdtoccvslb_by_crate Failed to register histogram " );       
            return sc;
          }
        }
        else if(iecap==enumEndCapA){
          sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_by_crate[iecap][ilayer],lbCrate_histtitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_ecA_shift);
          if(sc.isFailure()) {
            ATH_MSG_ERROR("m_mdtoccvslb_by_crate Failed to register histogram " );       
            return sc;
          }
        }
        else{
          sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_by_crate[iecap][ilayer],lbCrate_histtitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_ecC_shift);
          if(sc.isFailure()) {
            ATH_MSG_ERROR("m_mdtoccvslb_by_crate Failed to register histogram " );       
            return sc;
          }
        }

        sc = binMdtOccVsLB_Crate(m_mdtoccvslb_by_crate[iecap][ilayer],iecap,ilayer);
        if(sc.isFailure()) {
          ATH_MSG_ERROR("m_mdtoccvslb_by_crate Failed to register histogram " );       
          return sc;
        }

	std::string lbCrate_ontrack_histtitle = "OccupancyVsLB_ontrack_"+ecap[iecap]+crate[ilayer];
	
	if(iecap==enumBarrelA){
	  sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_ontrack_by_crate[iecap][ilayer],lbCrate_ontrack_histtitle,"LB","[Eta,Phi]",834,1,2502,100,1,100,m_mg->mongroup_brA_shift);
	  if(sc.isFailure()) {
	    ATH_MSG_ERROR("mdtoccvslb_ontrack_by_crate Failed to register histogram " );
	    return sc;
	  }
	}
	else if(iecap==enumBarrelC){
	  sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_ontrack_by_crate[iecap][ilayer],lbCrate_ontrack_histtitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_brC_shift);
	  if(sc.isFailure()) {
	    ATH_MSG_ERROR("mdtoccvslb_ontrack_by_crate Failed to register histogram " );
	    return sc;
	  }
	}
	else if(iecap==enumEndCapA){
	  sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_ontrack_by_crate[iecap][ilayer],lbCrate_ontrack_histtitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_ecA_shift);
	  if(sc.isFailure()) {
	    ATH_MSG_ERROR("mdtoccvslb_ontrack_by_crate Failed to register histogram " );
	    return sc;
	  }
	}
	else{
	  sc = bookMDTHisto_OccVsLB(m_mdtoccvslb_ontrack_by_crate[iecap][ilayer],lbCrate_ontrack_histtitle,"LB","[Eta,Phi]",834,1,2502,100,0,100,m_mg->mongroup_ecC_shift);
	  if(sc.isFailure()) {
                 ATH_MSG_ERROR("mdtoccvslb_ontrack_by_crate Failed to register histogram " );
                 return sc;
	  }
	}
	
	sc = binMdtOccVsLB_Crate(m_mdtoccvslb_ontrack_by_crate[iecap][ilayer],iecap,ilayer);
	if(sc.isFailure()) {
	  ATH_MSG_ERROR("mdtoccvslb_by_crate Failed to register histogram " );       
	  return sc;
	}
	
	
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      //Finished number of MDT hits per multilayer
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      for(int iPhi = 0; iPhi < 16; ++iPhi) {
        std::string Phi = returnString(iPhi+1);//iPhi starts from zero

        //////////////////////////// MDThitsOccup_inPhiSlice     
        if(ilayer==0 && iecap==0){
          //Initialize to zero
          m_mdtchamberstatphislice[iPhi] = 0;
          if(m_do_mdtchamberstatphislice) {
            sc = bookMDTHisto_overview(m_mdtchamberstatphislice[iPhi], "MDTHitsOccup_ADCCut_Sector"+Phi, "", "Counts/Chamber", 1, 0., 1., m_mg->mongroup_sectors_expert);
          }
        }

        //////////////////////////// MDT Summary plots Barrel-EndCap 
        if( !( iecap<=1 && ilayer==enumExtra && iPhi%2!=1 ) ) { // for barrel no odd-sectored extra layers            // !(ilayer==enumExtra && iecap>=2) ) { //don't do layer 'Extra' for endcaps
          std::string title_MDTHitSummary="MDTHits_ADCCut_"+ecap[iecap]+"_"+layer[ilayer]+"_StPhi"+Phi;
          int max=(iecap<2&&ilayer==0)?11:7;
          int nbins =(iecap<2&&ilayer==0)?10:6;

          //Initialize to zero
          m_mdtChamberHits[iecap][ilayer][iPhi] = 0;
          if(m_do_mdtChamberHits){
            if(iecap==0) sc=bookMDTHisto_overview(m_mdtChamberHits[iecap][ilayer][iPhi], title_MDTHitSummary.c_str(),
                "StationEta", "Counts/Chamber", nbins, 1, max, m_mg->mongroup_brA_hits_expert);
            if(iecap==1) sc=bookMDTHisto_overview(m_mdtChamberHits[iecap][ilayer][iPhi], title_MDTHitSummary.c_str(),
                "StationEta", "Counts/Chamber", nbins, 1, max, m_mg->mongroup_brC_hits_expert);
            if(iecap==2) sc=bookMDTHisto_overview(m_mdtChamberHits[iecap][ilayer][iPhi], title_MDTHitSummary.c_str(),
                "StationEta", "Counts/Chamber", nbins, 1, max, m_mg->mongroup_ecA_hits_expert);
            if(iecap==3) sc=bookMDTHisto_overview(m_mdtChamberHits[iecap][ilayer][iPhi], title_MDTHitSummary.c_str(),
                "StationEta", "Counts/Chamber", nbins, 1, max, m_mg->mongroup_ecC_hits_expert);
          }

          if(sc.isFailure()) { 
            ATH_MSG_ERROR("m_mdtChamberHits per eta and phi Failed to register histogram " );       
            return sc;
          }    

          //ATH_MSG_DEBUG("SHIFT : " << shift );
          ATH_MSG_DEBUG("RUN : " << run );
          /////////////////////////////////////////MDT Summary plots Barrel-Endcap

          /////////////////////////////////////////MDTTDC summary sector by sector
          std::string title_MDTTDCSummary="MDTTDC_ADCCut_"+ecap[iecap]+"_"+layer[ilayer]+"_StPhi"+Phi;

          //Initialize to zero
          m_mdttdccut_sector[iecap][ilayer][iPhi] = 0;
          if(m_do_mdttdccut_sector){
            if(iecap==0) sc=bookMDTHisto_overview(m_mdttdccut_sector[iecap][ilayer][iPhi], title_MDTTDCSummary.c_str(),
                                                  "[nsec]", "number of entries", 100, 0, 2000., m_mg->mongroup_brA_tdc_expert);
            if(iecap==1) sc=bookMDTHisto_overview(m_mdttdccut_sector[iecap][ilayer][iPhi], title_MDTTDCSummary.c_str(),
                                                  "[nsec]", "number of entries", 100, 0, 2000., m_mg->mongroup_brC_tdc_expert);
            if(iecap==2) sc=bookMDTHisto_overview(m_mdttdccut_sector[iecap][ilayer][iPhi], title_MDTTDCSummary.c_str(),
                                                  "[nsec]", "number of entries", 100, 0, 2000., m_mg->mongroup_ecA_tdc_expert);
            if(iecap==3) sc=bookMDTHisto_overview(m_mdttdccut_sector[iecap][ilayer][iPhi], title_MDTTDCSummary.c_str(),
                                                  "[nsec]", "number of entries", 100, 0, 2000., m_mg->mongroup_ecC_tdc_expert);
          }

          if(sc.isFailure()) { 
            ATH_MSG_ERROR("m_mdttdccut_sector per eta and phi Failed to register histogram " );       
            return sc;
          }     
        }

      } // loop in phi
      } // loop in layer
    } // loop in ecap 
  }//newRunFlag()

  ATH_MSG_DEBUG("LEAVING MDTSUMMARYBOOK");
  return sc;
}

StatusCode MdtRawDataValAlg::bookMDTOverviewHistograms(/* bool isNewEventsBlock,*/ bool newLumiBlock, bool newRun) {
  StatusCode sc = StatusCode::SUCCESS;

  //if( isNewEventsBlock ){}
  if( newLumiBlock ){}
  //book these histos as lowStat interval if not online monitoring
  if( (newLowStatIntervalFlag() && !m_isOnline) || (m_isOnline && newRun) ){
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdccut spectrum
    sc = bookMDTHisto_overview(m_overalltdccutLumi, "Overall_TDC_ADCCut_spectrum", "[nsec]", "Number of Entries",
                               120, 0., 2000., m_mg->mongroup_overview_shiftLumi);

    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdccut spectrum (along segms)
    sc = bookMDTHisto_overview(m_overalltdccut_segm_Lumi, "Overall_TDC_onSegm_ADCCut_spectrum", "[nsec]", "Number of Entries", 
                               120, 0., 2000., m_mg->mongroup_overview_shiftLumi);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall adc spectrum (along segms)
    sc = bookMDTHisto_overview(m_overalladc_segm_Lumi, "Overall_ADC_onSegm_spectrum", "[adc counts]", "Number of Entries", 
                               100, 0., 400., m_mg->mongroup_overview_shiftLumi);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall adc spectrum
    sc = bookMDTHisto_overview(m_overalladc_Lumi, "Overall_ADC_spectrum", "[adc counts]", "Number of Entries", 
                               100, 0., 400., m_mg->mongroup_overview_shiftLumi);

    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event with a cut on ADC 
    sc = bookMDTHisto_overview(m_mdteventscutLumi, "TotalNumber_of_MDT_hits_per_event_ADCCut", "[counts]", "Number of Events",
                               400, 0., 10000., m_mg->mongroup_overview_shiftLumi);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event with a cut on ADC  (for high mult. events)
    sc = bookMDTHisto_overview(m_mdteventscutLumi_big, "TotalNumber_of_MDT_hits_per_event_big_ADCCut", "[counts]", "Number of Events",
                               200, 0., 100000., m_mg->mongroup_overview_shiftLumi);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event without a cut on ADC 
    sc = bookMDTHisto_overview(m_mdteventsLumi, "TotalNumber_of_MDT_hits_per_event", "[counts]", "Number of Events",
                               500, 0., 20000., m_mg->mongroup_overview_shiftLumi);
    
     
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event without a cut on ADC  (for high mult. events)
    sc = bookMDTHisto_overview(m_mdteventsLumi_big, "TotalNumber_of_MDT_hits_per_event_big", "[counts]", "Number of Events",
                               200, 0., 100000., m_mg->mongroup_overview_shiftLumi);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for TotalNumber_of_MDT_hits_per_event_RPCtrig 
    /* sc = bookMDTHisto_overview(mdtevents_RPCtrig, "TotalNumber_of_MDT_hits_per_event_RPCtrig_ADCCut", "[counts]",
       "Number of Events", 200, 0., 800., m_mg->mongroup_overview_shiftLumi);

       ////////////////////////////////////////////////////////////////////////////////////// 
       //histo path for TotalNumber_of_MDT_hits_per_event_TGCtrig 
       sc = bookMDTHisto_overview(mdtevents_TGCtrig, "TotalNumber_of_MDT_hits_per_event_TGCtrig_ADCCut", "[counts]",
       "Number of Events", 200, 0., 800., m_mg->mongroup_overview_shiftLumi);

       ////////////////////////////////////////////////////////////////////////////////////// 
       //histo path for overall tdccut RPCtrig spectrum 
       sc = bookMDTHisto_overview(m_overalltdccut_RPCtrig, "Overall_TDC_ADCCut_RPCtrig_spectrum", "[nsec]", "Number of Entries",
       120, 0., 2000., m_mg->mongroup_overview_shiftLumi);

       ////////////////////////////////////////////////////////////////////////////////////// 
       //histo path for overall tdccut TGCtrig spectrum 
       sc = bookMDTHisto_overview(m_overalltdccut_TGCtrig, "Overall_TDC_ADCCut_TGCtrig_spectrum", "[nsec]", "Number of Entries",
       120, 0., 2000., m_mg->mongroup_overview_shiftLumi);
   
    */
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdc vs adc spectrum 
    sc = bookMDTHisto_overview_2D(m_overalltdcadcLumi, "Overall_TDCADC_spectrum", "[nsec]", "[adc counts]", 50, 0, 2000.,
                                  20. ,0., 400., m_mg->mongroup_overview_shiftLumi);  


  }

  if(newRun){
	  
    //histo path for Time_large_global_hits with a cut on ADC 
    sc = bookMDTHisto_overview(m_mdtglobalhitstime, "Time_large_global_hits", "time", "Number of Events",
                               10000, 0., 10000., m_mg->mongroup_overview_shift);
    /*
    //histo path for TotalNumber_of_MDT_hits_vs_event_number without a cut on ADC 
    sc = bookMDTHisto_overview(mdthitsvseventnum, "TotalNumber_of_MDT_hits_vs_event_number", "event #", "Number of Hits",
    10000, 0., 10000., m_mg->mongroup_overview_shift);
    //histo path for TotalNumber_of_MDT_hits_vs_event_number with a cut on ADC 
    sc = bookMDTHisto_overview(mdthitsvseventnumcut, "TotalNumber_of_MDT_hits_vs_event_number_ADCCut", "event #", "Number of Hits",
    10000, 0., 10000., m_mg->mongroup_overview_shift);
    */
    //histo path for m_MdtNHitsvsRpcNHits
    sc = bookMDTHisto_overview_2D(m_MdtNHitsvsRpcNHits, "m_MdtNHitsvsRpcNHits", "# MDT hits","# RPC hits", 1000, 0., 100000., 100, 0., 10000., m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdccut spectrum
    sc = bookMDTHisto_overview(m_overalltdcHighOcc, "Overall_TDC_spectrum_NoiseBursts", "[nsec]", "Number of Entries",
                               120, 0., 2000., m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall adc spectrum
    sc = bookMDTHisto_overview(m_overalladc_HighOcc, "Overall_ADC_spectrum_NoiseBursts", "[adc counts]", "Number of Entries", 
                               100, 0., 400., m_mg->mongroup_overview_shift);
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for overall tdc vs adc spectrum 
    sc = bookMDTHisto_overview_2D(m_overalltdcadcHighOcc, "Overall_TDCADC_spectrum_NoiseBursts", "[nsec]", "[adc counts]", 50, 0, 2000.,
                                  20. ,0., 400., m_mg->mongroup_overview_shift);  
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for Number_of_MDTs_with_hits_per_event 
    sc = bookMDTHisto_overview(m_nummdtchamberswithhits, "number_of_Chambers_with_hits_per_event", "[Number_of_MDT_chambers_with_hits]",
                               "Number of Entries", 400, 0.,1600., m_mg->mongroup_overview_shift); 

    //histo path for Number_of_MDTs_with_hits_per_event 
    sc = bookMDTHisto_overview(m_nummdtchamberswithHighOcc, "number_of_Chambers_with_high_occupancy_per_event", "[Number_of_MDT_chambers_with_high_occupancy]",
                               "Number of Entries", 200, 0., 800., m_mg->mongroup_overview_shift); 
    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for Number_of_MDTs_with_hits_per_event_ADCCut 
    sc = bookMDTHisto_overview(m_nummdtchamberswithhits_ADCCut, "number_of_Chambers_with_hits_per_event_ADCCut",
                               "[Number_of_MDT_chambers_with_hits]", "Number of Entries", 400, 0., 1600., m_mg->mongroup_overview_shift);

    ////////////////////////////////////////////////////////////////////////////////////// 
    //Histo Path for Number_of_MDT_hits_per_chamber 
    sc = bookMDTHisto_overview(m_mdtchamberstat, "Number_of_MDT_hits_per_chamber_ADCCut", "MDTChamber", "Counts/Chamber",
                               1, 0., 1., m_mg->mongroup_overview_expert);
    for(vector<Identifier>::const_iterator itr = m_chambersId.begin(); itr != m_chambersId.end(); ++itr){
      std::string hardware_name = getChamberName( *itr );
      //       std::string hardware_name = convertChamberName(m_mdtIdHelper->stationName(*itr),m_mdtIdHelper->stationEta(*itr),
      //                 m_mdtIdHelper->stationPhi(*itr),"MDT");
      //Skip Chambers That Do NOT Exist
      if(hardware_name=="BML6A13" || hardware_name=="BML6C13") continue;
      m_mdtchamberstat->Fill( hardware_name.c_str(), 0.0);
    }
    m_mdtchamberstat->Reset();
    m_mdtchamberstat->LabelsOption("a");
    m_mdtchamberstat->GetXaxis()->LabelsOption("v");

    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for Number_of_MDTHits_inRZView_Global and Number_of_MDTHits_inYXView_Global for EndCap, Barrel and Overlap respectively
    std::string mdtreg[3]={"Barrel","Overlap", "EndCap"};    
    std::string generic_path_mdtrzxydet = "MDT/Overview";

    for (int imdtreg=0;imdtreg<3;imdtreg++) {     
      ///FOR R-Z view
      sc = bookMDTHisto_overview_2D(m_mdtrzdet[imdtreg], "Number_of_"+mdtreg[imdtreg]+"MDTHits_inRZView_Global_ADCCut",
                                    "MDT-GlobalZ(mm)", "MDT-GlobalR(mm)", 250, -25000., 25000., 120, 0., 12000., m_mg->mongroup_overview_shift_geometry);

      ///FOR X-Y view
      sc = bookMDTHisto_overview_2D(m_mdtxydet[imdtreg], "Number_of_"+mdtreg[imdtreg]+"MDTHits_inYXView_Global_ADCCut",
                                    "MDT-GlobalX(mm)", "MDT-GlobalY(mm)", 150, -15000., 15000., 150, -15000., 15000., m_mg->mongroup_overview_shift_geometry);
    }

  }//newRun
  return sc;
}

StatusCode MdtRawDataValAlg::fillMDTHistograms( const Muon::MdtPrepData* mdtCollection ) {
  //fill chamber by chamber histos
  StatusCode sc = StatusCode::SUCCESS;
  Identifier digcoll_id = (mdtCollection)->identify();
  IdentifierHash digcoll_idHash = (mdtCollection)->collectionHash();

  MDTChamber* chamber;
  sc = getChamber( digcoll_idHash, chamber );
  if(!sc.isSuccess()){
    ATH_MSG_ERROR( "Could Not Retrieve MDTChamber w/ ID " << digcoll_idHash );
    return sc;
  }

  std::string hardware_name = chamber->getName();

  //      //convert layer numbering from 1->4 to 1->8
  //      //check if we are in 2nd multilayer
  //      //then add 4 if large chamber, 3 if small chamber 
  int mdtlayer = m_mdtIdHelper->tubeLayer(digcoll_id);
  if (m_mdtIdHelper->multilayer(digcoll_id)==2) {
    if ( hardware_name.at(1) == 'I' && hardware_name.at(3) != '8' )
      mdtlayer += 4;
    else 
      mdtlayer += 3;
  }   
  int mdttube= m_mdtIdHelper->tube(digcoll_id) + (mdtlayer-1) * m_mdtIdHelper->tubeMax(digcoll_id);
  ChamberTubeNumberCorrection(mdttube, hardware_name, m_mdtIdHelper->tube(digcoll_id), mdtlayer-1);

  bool isNoisy = m_masked_tubes->isNoisy( mdtCollection );
  //   //// Verifiy back-conversion of tubeID -> tube/layer/ML
  //   int test_tube = 0;
  //   int test_layer = 0;
  //   int test_ML = 0;
  //   TubeID_to_ID_L_ML(mdttube, hardware_name, test_tube, test_layer, test_ML, GetTubeMax(digcoll_id,hardware_name));
  //   if( test_tube != m_mdtIdHelper->tube(digcoll_id) || test_layer != m_mdtIdHelper->tubeLayer(digcoll_id) || test_ML != m_mdtIdHelper->multilayer(digcoll_id) ) {
  //     ATH_MSG_DEBUG("FAIL:" << hardware_name << ": true (ML,tubeLayer,tube): (" << m_mdtIdHelper->multilayer(digcoll_id) << ", " << m_mdtIdHelper->tubeLayer(digcoll_id) << ", " << m_mdtIdHelper->tube(digcoll_id) << "), derived: (" << test_ML << ", " << test_layer << ", " << test_tube << "), [" << m_mdtIdHelper->tubeMax(digcoll_id) << "," << m_mdtIdHelper->tubeLayerMax(digcoll_id) << "]");
  //     if( (hardware_name.substr(0,4) == "BIR1" || hardware_name.substr(0,4) == "BIR4") && m_mdtIdHelper->multilayer(digcoll_id) == 1 ) {
  //       ATH_MSG_DEBUG(" Probably OK: shift due to cutout!" );      
  //     }
  //     else ATH_MSG_DEBUG("\n");
  //   }
  //   //// end back-conversion verification

  std::string tube_str = returnString(mdttube);


  float tdc = mdtCollection->tdc()*25.0/32.0;
  // Note: the BMG is digitized with 200ps which is not same as other MDT chambers with 25/32=781.25ps
  if(hardware_name.substr(0,3)=="BMG") tdc = mdtCollection->tdc() * 0.2;
  float adc = mdtCollection->adc();
  if(hardware_name.substr(0,3) == "BMG") adc /= 4.;

  if (chamber->mdttdc) {
    chamber->mdttdc->Fill(tdc); 
  }
  else { ATH_MSG_DEBUG("mdttdc not in hist list!" ); }

  int mdtMultLayer = m_mdtIdHelper->multilayer(digcoll_id);

  // trigger specific
  if ( adc >m_ADCCut && !isNoisy ) {
    if (chamber->mdttdccut_ML1 && mdtMultLayer==1) { chamber->mdttdccut_ML1->Fill(tdc); }
    if (chamber->mdttdccut_ML2 && mdtMultLayer==2) { chamber->mdttdccut_ML2->Fill(tdc); }
    //if (chamber->mdttdccut_RPCtrig_ML1 && HasTrigBARREL() && mdtMultLayer==1) { chamber->mdttdccut_RPCtrig_ML1->Fill(tdc); }
    //if (chamber->mdttdccut_TGCtrig_ML1 && HasTrigENDCAP() && mdtMultLayer==1) { chamber->mdttdccut_TGCtrig_ML1->Fill(tdc); }
    //if (chamber->mdttdccut_RPCtrig_ML2 && HasTrigBARREL() && mdtMultLayer==2) { chamber->mdttdccut_RPCtrig_ML2->Fill(tdc); }
    //if (chamber->mdttdccut_TGCtrig_ML2 && HasTrigENDCAP() && mdtMultLayer==2) { chamber->mdttdccut_TGCtrig_ML2->Fill(tdc); }
  }

  if (chamber->mdtadc) { chamber->mdtadc->Fill(adc); }

  if (chamber->mdttdcadc && adc > 0) { chamber->mdttdcadc->Fill(tdc, adc); }

  if (chamber->mdtlayer) { if((adc >m_ADCCut && !isNoisy)) chamber->mdtlayer->Fill(mdtlayer); }

  if (chamber->mdttube) { if((adc >m_ADCCut) ) chamber->mdttube->Fill(mdttube); }
/*
  if (chamber->mdttube_bkgrd ) { 
    if(adc > m_ADCCut_Bkgrd && tdc < m_TDCCut_Bkgrd) chamber->mdttube_bkgrd->Fill(m_mdttube); 
  }
  if (chamber->mdtmultil) { if((mdtCollection->adc()>m_ADCCut && !isNoisy)) chamber->mdtmultil->Fill(m_mdtIdHelper->multilayer(digcoll_id)); }

    if (chamber->mdttube_fornoise) { if(tdc < m_TDCCut_Bkgrd) chamber->mdttube_fornoise->Fill(mdttube); }
  */
  if (chamber->mdtmezz) { if( adc > m_ADCCut) chamber->mdtmezz->Fill( mezzmdt( digcoll_id ) ); }

  return sc;
}

//StatusCode MdtRawDataValAlg::fillMDTSummaryHistograms( const Muon::MdtPrepData* mdtCollection, bool &isNoiseBurstCandidate ) {
StatusCode MdtRawDataValAlg::fillMDTSummaryHistograms( const Muon::MdtPrepData* mdtCollection, std::set<std::string>  chambers_from_tracks, bool &isNoiseBurstCandidate ) {

  StatusCode sc = StatusCode::SUCCESS;
  Identifier digcoll_id = (mdtCollection)->identify();
  IdentifierHash digcoll_idHash = (mdtCollection)->collectionHash();

  MDTChamber* chamber;
  sc = getChamber( digcoll_idHash, chamber );
  if(!sc.isSuccess()){
    ATH_MSG_ERROR( "Could Not Retrieve MDTChamber w/ ID " << digcoll_idHash );
    return sc;
  }
  bool isNoisy = m_masked_tubes->isNoisy( mdtCollection );

  int ibarrel = chamber->GetBarrelEndcapEnum();
  int iregion = chamber->GetRegionEnum();
  int ilayer = chamber->GetLayerEnum();
  int icrate = chamber->GetCrate();
  int stationEta = chamber->GetStationEta(); 
  int stationPhi = chamber->GetStationPhi();
  std::string chambername = chamber->getName();
  bool is_on_track = false;
  for(auto ch : chambers_from_tracks) {
    if(chambername==ch) is_on_track=true;
  }
  bool isBIM = (chambername.at(2)=='M');

  float tdc = mdtCollection->tdc()*25.0/32.0;
  // Note: the BMG is digitized with 200ps which is not same as other MDT chambers with 25/32=781.25ps
  if(chambername.substr(0,3)=="BMG") tdc = mdtCollection->tdc() * 0.2;
  float adc = mdtCollection->adc();
  if(chambername.substr(0,3) == "BMG") adc /= 4.;

  if( m_mdtChamberHits[iregion][ilayer][stationPhi] && adc > m_ADCCut )
    m_mdtChamberHits[iregion][ilayer][stationPhi]->Fill(std::abs(stationEta));

  int mlayer_n = m_mdtIdHelper->multilayer(digcoll_id);

  // Fill Barrel - Endcap Multilayer Hits
  if(!isNoisy && adc > 0){
    m_overalltdcadcPRLumi[iregion]->Fill(tdc, adc);
    if(isNoiseBurstCandidate) m_overalltdcadcPR_HighOcc[iregion]->Fill(tdc,adc);
  }
  if(!isNoisy){
    m_overalladcPRLumi[iregion]->Fill(adc);
    if(isNoiseBurstCandidate){
      m_overalltdcPR_HighOcc[iregion]->Fill(tdc);
      m_overalladcPR_HighOcc[iregion]->Fill(adc);
    }
  }
  if( adc >m_ADCCut && !isNoisy) {

    m_mdthitsperchamber_InnerMiddleOuterLumi[ibarrel]->AddBinContent( chamber->GetMDTHitsPerChamber_IMO_Bin(), 1. );
    m_mdthitsperchamber_InnerMiddleOuterLumi[ibarrel]->SetEntries(m_mdthitsperchamber_InnerMiddleOuterLumi[ibarrel]->GetEntries()+1);
    
    m_mdthitspermultilayerLumi[iregion][ilayer]->AddBinContent(chamber->GetMDTHitsPerML_Bin(mlayer_n), 1. );
    m_mdthitspermultilayerLumi[iregion][ilayer]->SetEntries(m_mdthitspermultilayerLumi[iregion][ilayer]->GetEntries()+1); 
    
    //We put the "extras" on the inner histogram to avoid creating separate histogram.
    m_mdthitsperML_byLayer[(ilayer < 3 ? ilayer : enumInner)]->AddBinContent(chamber->GetMDTHitsPerML_byLayer_Bin(mlayer_n), 1.) ;
    m_mdthitsperML_byLayer[(ilayer < 3 ? ilayer : enumInner)]->SetEntries(m_mdthitsperML_byLayer[(ilayer < 3 ? ilayer : enumInner)]->GetEntries()+1) ;
  
    m_overalladccutPRLumi[iregion]->Fill(adc);  
    m_overalltdccutPRLumi[iregion]->Fill(tdc);
    if(isNoiseBurstCandidate){
      m_mdthitsperchamber_InnerMiddleOuter_HighOcc[ibarrel]->AddBinContent( chamber->GetMDTHitsPerChamber_IMO_Bin(), 1. );
      m_mdthitsperchamber_InnerMiddleOuter_HighOcc[ibarrel]->SetEntries(m_mdthitsperchamber_InnerMiddleOuter_HighOcc[ibarrel]->GetEntries()+1);
      m_overalltdcPR_HighOcc_ADCCut[iregion]->Fill(tdc);
    }
      
    if( HasTrigBARREL() ) m_overalltdccutPRLumi_RPCtrig[iregion]->Fill(tdc);
    if( HasTrigENDCAP() ) m_overalltdccutPRLumi_TGCtrig[iregion]->Fill(tdc);

    //
    // Fill occupancy vs. Lumiblock
    if(ilayer != 3) m_mdtoccvslb[iregion][ilayer]->Fill(m_lumiblock,get_bin_for_LB_hist(iregion,ilayer,stationPhi,stationEta,isBIM));
    else m_mdtoccvslb[iregion][2]->Fill(m_lumiblock,get_bin_for_LB_hist(iregion,ilayer,stationPhi,stationEta,isBIM)); // Put extras in with outer
    
    //correct readout crate info for BEE,BIS7/8
    int crate_region = iregion;
    if(chambername.substr(0,3)=="BEE" || (chambername.substr(0,3) == "BIS" && (stationEta == 7 || stationEta == 8) )){
      if(iregion==0) crate_region=2;
      if(iregion==1) crate_region=3;
    }
    //use stationPhi+1 becuase that's the actual phi, not phi indexed from zero.
    m_mdtoccvslb_by_crate[crate_region][icrate-1]->Fill(m_lumiblock,get_bin_for_LB_crate_hist(crate_region,icrate,stationPhi+1,stationEta,chambername));

    if (is_on_track)    {
      m_mdtoccvslb_ontrack_by_crate[crate_region][icrate-1]->Fill(m_lumiblock,get_bin_for_LB_crate_hist(crate_region,icrate,stationPhi+1,stationEta,chambername));
    }

  }  

  return sc;
}

StatusCode MdtRawDataValAlg::fillMDTOverviewHistograms( const Muon::MdtPrepData* mdtCollection, bool &isNoiseBurstCandidate ) {
  StatusCode sc = StatusCode::SUCCESS;
  Identifier digcoll_id = (mdtCollection)->identify();
  //  std::string type="MDT";
  //   std::string hardware_name = convertChamberName(m_mdtIdHelper->stationName(digcoll_id),
  //             m_mdtIdHelper->stationEta(digcoll_id),
  //             m_mdtIdHelper->stationPhi(digcoll_id),type);

  std::string hardware_name = getChamberName( mdtCollection );
  bool isNoisy = m_masked_tubes->isNoisy( mdtCollection );

  const MuonGM::MdtReadoutElement* pReadoutElementMDT = p_MuonDetectorManager->getMdtReadoutElement(digcoll_id);
  const Amg::Vector3D mdtgPos = pReadoutElementMDT->tubePos(digcoll_id); //global position of the wire                  
  float mdt_tube_eta   = mdtgPos.eta();     
  float mdt_tube_x = mdtgPos.x();
  float mdt_tube_y = mdtgPos.y();
  float mdt_tube_z = mdtgPos.z();
  float mdt_tube_perp = mdtgPos.perp();

  float tdc = mdtCollection->tdc()*25.0/32.0;
  // Note: the BMG is digitized with 200ps which is not same as other MDT chambers with 25/32=781.25ps
  if(hardware_name.substr(0,3)=="BMG") tdc = mdtCollection->tdc() * 0.2;
  float adc = mdtCollection->adc();
  if(hardware_name.substr(0,3) == "BMG") adc /= 4.;

  //Barrel -->Fill MDT Global RZ and YX
  if( adc>m_ADCCut ) {
    if(fabs(mdt_tube_eta)>0. && fabs(mdt_tube_eta)<0.9){m_mdtrzdet[0]->Fill(mdt_tube_z,mdt_tube_perp); m_mdtxydet[0]->Fill(mdt_tube_x,mdt_tube_y);}     
    //OverLap -->Fill MDT Global RZ and YX
    if(fabs(mdt_tube_eta)>0.9 && fabs(mdt_tube_eta)<1.2){m_mdtrzdet[1]->Fill(mdt_tube_z,mdt_tube_perp); m_mdtxydet[1]->Fill(mdt_tube_x,mdt_tube_y);}      
    //EndCap -->Fill MDT Global RZ and YX
    if(fabs(mdt_tube_eta)>1.2 && fabs(mdt_tube_eta)<2.7){m_mdtrzdet[2]->Fill(mdt_tube_z,mdt_tube_perp); m_mdtxydet[2]->Fill(mdt_tube_x,mdt_tube_y);}      
  }

  if(m_overalltdcadcLumi && !isNoisy && adc > 0) m_overalltdcadcLumi->Fill(tdc, adc);
  else if(!m_overalltdcadcLumi) ATH_MSG_DEBUG("m_overalltdcadcLumi not in hist list!" );

  if(m_overalladc_Lumi) { m_overalladc_Lumi->Fill(adc); }
  else ATH_MSG_DEBUG("m_overalladc_Lumi not in hist list!" );

  if(isNoiseBurstCandidate){
    if(m_overalladc_HighOcc && !isNoisy) { m_overalladc_HighOcc->Fill(adc); }
    else ATH_MSG_DEBUG("m_overalladc_HighOcc not in hist list!" );
    if(m_overalltdcadcHighOcc && adc > 0) { m_overalltdcadcHighOcc->Fill(tdc, adc); }
    else ATH_MSG_DEBUG("m_overalltdcadcHighOcc not in hist list!" );
    if(m_overalltdcHighOcc) { m_overalltdcHighOcc->Fill(tdc); }
    else ATH_MSG_DEBUG("m_overalltdcHighOcc not in hist list!" );
    if(m_overalltdcHighOcc_ADCCut && adc > m_ADCCut) { m_overalltdcHighOcc_ADCCut->Fill(tdc); }
    else ATH_MSG_DEBUG("m_overalltdcHighOcc_ADCCut not in hist list!" );
  }
  
  if (adc>m_ADCCut) {
    if(m_overalltdccutLumi && !isNoisy) m_overalltdccutLumi->Fill(tdc);
    if(!m_overalltdccutLumi) ATH_MSG_DEBUG("overalltdccut not in hist list");
  }
/*  if (adc>m_ADCCut && HasTrigBARREL()) {
    if(m_overalltdccut_RPCtrig) m_overalltdccut_RPCtrig->Fill(tdc);
    else ATH_MSG_DEBUG("overalltdccut not in hist list!" );	
  }
  if (adc>m_ADCCut && HasTrigENDCAP()) {
    if(m_overalltdccut_TGCtrig) m_overalltdccut_TGCtrig->Fill(tdc);
    else ATH_MSG_DEBUG("overalltdccut not in hist list!" );
  }
*/
  return sc;
} 

// Code for measuring tube efficiencies and tdc/adc based on hits along segments  
// Strategy: 
// First loop over hits along segments and store hits
// Identify the MLs affected for each segment
// Loop over the tubes in the affected MLs and identify tubes traversed by segment vector (these represent the denom in the efficiency calc)
// Find traversed tubes that also have a hit along the segment (these represent the num in the efficiency calc)
// Details: 
//        * To avoid double-counting hits (for tdc/adc fills) due to overlapping segments, use a set called store_ROTs 
//        * To avoid double-counting hits (for eff calc) due to overlapping segments, use a set called store_effTubes
//        * The above 2 sets need not have the same size, because in the latter case some tubes are missed because they are slightly too
//          far away from the segment vector -- these tubes are simply excluded from the eff calc. 
//          Additionally the latter case is complicated because for overlapping traversed tubes, 
//          we must preference the ones that are part of a segment that records a hit in those tubes  
StatusCode MdtRawDataValAlg::handleEvent_effCalc(const Trk::SegmentCollection* segms){//, const Muon::MdtPrepDataContainer* mdt_container) {
  StatusCode sc=StatusCode::SUCCESS;
  std::string type="MDT";
  std::set<TubeTraversedBySegment, TubeTraversedBySegment_cmp> store_effTubes;
  std::set<Identifier> store_ROTs;

  // LOOP OVER SEGMENTS  
  for (Trk::SegmentCollection::const_iterator s = segms->begin(); s != segms->end(); ++s) {
    const Muon::MuonSegment* segment = dynamic_cast<const Muon::MuonSegment*>(*s);
    if (segment == 0) {
      ATH_MSG_DEBUG("no pointer to segment!!!");
      break;
    }    
    if(segment->containedROTs().size() < m_nb_hits || segment->containedROTs().size() <= 0 || segment->fitQuality()->chiSquared() / segment->fitQuality()->doubleNumberDoF() > m_chi2_cut) {
      continue;
    }

    // Gather hits used in segment
    std::vector<Identifier> ROTs_chamber;
    std::vector<int> ROTs_tube;
    std::vector<int> ROTs_L;
    std::vector<int> ROTs_ML;
    std::vector<float> ROTs_DR;
    std::vector<float> ROTs_DRerr;
    std::vector<float> ROTs_DT;
    const std::vector<const Trk::RIO_OnTrack*>& rots = segment->containedROTs();
    std::vector<const Trk::RIO_OnTrack*>::const_iterator rit = rots.begin();
    std::vector<const Trk::RIO_OnTrack*>::const_iterator rit_end = rots.end();
    for( ; rit!=rit_end;++rit ) {
      const Trk::RIO_OnTrack* rot = *rit;
      const Muon::MdtDriftCircleOnTrack* mrot = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(rot);
      if(mrot) {
    	  
        Identifier tmpid = rot->identify();
        IdentifierHash idHash;
        MDTChamber* chamber = 0;
        m_mdtIdHelper->get_module_hash( tmpid, idHash );  
        sc = getChamber(idHash, chamber);
        std::string chambername = chamber->getName();
        float adc = mrot->prepRawData()->adc();
        if(chambername.substr(0,3)=="BMG") adc /= 4. ;
        if(m_overalladc_segm_Lumi) m_overalladc_segm_Lumi->Fill(adc);
        if( store_ROTs.find(tmpid) == store_ROTs.end() ) { // Let's not double-count hits belonging to multiple segments
          store_ROTs.insert(tmpid);   

          double tdc = mrot->prepRawData()->tdc()*25.0/32.0;
          // Note: the BMG is digitized with 200ps which is not same as other MDT chambers with 25/32=781.25ps
          if(chambername.substr(0,3)=="BMG") tdc = mrot->prepRawData()->tdc() * 0.2;
              //      double tdc = mrot->driftTime()+500;
              int iregion = chamber->GetRegionEnum();
              int ilayer = chamber->GetLayerEnum();
              int statphi = chamber->GetStationPhi();
              int ibarrel_endcap = chamber->GetBarrelEndcapEnum();
              if(m_overalladc_segm_PR_Lumi[iregion] && adc > m_ADCCut) m_overalladc_segm_PR_Lumi[iregion]->Fill(adc);        

              if(adc > m_ADCCut) { // This is somewhat redundant because this is usual cut for segment-reconstruction, but that's OK
                if(statphi > 15) {
                  ATH_MSG_ERROR( "MDT StationPhi: " << statphi << " Is too high.  Chamber name: " << chamber->getName() );
                  continue;
                }

                if( m_mdttdccut_sector[iregion][ilayer][statphi] ) m_mdttdccut_sector[iregion][ilayer][statphi]->Fill(tdc);
            //Fill Overview Plots
            if(m_overalltdccut_segm_Lumi) m_overalltdccut_segm_Lumi->Fill(tdc);
            if(m_overalltdccut_segm_PR_Lumi[iregion]) m_overalltdccut_segm_PR_Lumi[iregion]->Fill(tdc);

            if(m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[ibarrel_endcap]) {
              m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[ibarrel_endcap]->AddBinContent( chamber->GetMDTHitsPerChamber_IMO_Bin(), 1. );
              m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[ibarrel_endcap]->SetEntries(m_mdthitsperchamber_onSegm_InnerMiddleOuterLumi[ibarrel_endcap]->GetEntries()+1);
            }
          }
          int mdtMultLayer = m_mdtIdHelper->multilayer(tmpid);
          //chamber->mdtadc_onSegm->Fill(mrot->prepRawData()->adc());
  	  if(chamber->mdtadc_onSegm_ML1 && mdtMultLayer == 1){
        	  chamber->mdtadc_onSegm_ML1->Fill(adc); 
          	  //sumADC_ML1 += mrot->prepRawData()->adc();
	  	 // numHits_ML1++;
	  }
          if(chamber->mdtadc_onSegm_ML2&& mdtMultLayer == 2){
        	  chamber->mdtadc_onSegm_ML2->Fill(adc);       	  
     		 // sumADC_ML2 += mrot->prepRawData()->adc();
		 // numHits_ML2++;
 	     }
        }
        // This information needs to be stored fully for each segment (for calculations below), so deal with these duplicates later
        // (otherwise we may not check a traversed ML for a differently pointing overlapping segment, for example)

        ROTs_chamber.push_back( tmpid );
        ROTs_ML.push_back( m_mdtIdHelper->multilayer(tmpid) );
        ROTs_L.push_back( m_mdtIdHelper->tubeLayer(tmpid) );
        ROTs_tube.push_back( m_mdtIdHelper->tube(tmpid) );
        ROTs_DR.push_back( mrot->driftRadius() );
        ROTs_DRerr.push_back( (mrot->localCovariance())(Trk::driftRadius,Trk::driftRadius) ) ;       // always returns value 2.0
        ROTs_DT.push_back( mrot->driftTime() );
      }
    }
    // Finished gathering hits used in segment

    if(m_doChamberHists) { //Don't perform this block if not doing chamber by chamber hists

      // Find unique chambers (since above we stored one chamber for every tube)
      // Also store the MLs affected by the ROTs, since we don't necessarily want to look for traversed tubes in entire chamber
      std::vector<Identifier> unique_chambers;
      std::vector<vector<int> > unique_chambers_ML;
      for(unsigned i=0; i<ROTs_chamber.size(); i++) {
        bool isUnique = true;
        for(unsigned j=0; j<unique_chambers.size(); j++) {
          if( getChamberName(ROTs_chamber.at(i)) == getChamberName(unique_chambers.at(j)) ){
            //    if( convertChamberName(m_mdtIdHelper->stationName(ROTs_chamber.at(i)), m_mdtIdHelper->stationEta(ROTs_chamber.at(i)), m_mdtIdHelper->stationPhi(ROTs_chamber.at(i)),type)
            //        == convertChamberName(m_mdtIdHelper->stationName(unique_chambers.at(j)), m_mdtIdHelper->stationEta(unique_chambers.at(j)), m_mdtIdHelper->stationPhi(unique_chambers.at(j)),type) ) {
            isUnique = false;
            if( !AinB( ROTs_ML.at(i), unique_chambers_ML.at(j) ) ) 
              unique_chambers_ML.at(j).push_back( ROTs_ML.at(i) );
            break;
          }   
        }
        if(isUnique) { 
          unique_chambers.push_back( ROTs_chamber.at(i) );
          std::vector<int> tmp_ML; tmp_ML.push_back( ROTs_ML.at(i) ); unique_chambers_ML.push_back( tmp_ML );
        }
      }
      // Done finding unique chambers

      // Loop over the unique chambers
      // Here we store the tubes in each chamber that were traversed by the segment
      std::vector<Identifier> traversed_station_id;
      std::vector<int> traversed_tube;
      std::vector<int> traversed_L;
      std::vector<int> traversed_ML;    
      std::vector<float> traversed_distance;    
      for( unsigned i_chamber=0; i_chamber<unique_chambers.size(); i_chamber++) {
        Identifier station_id = unique_chambers.at(i_chamber);
        if( !m_mdtIdHelper->is_mdt( station_id ) ) {
          ATH_MSG_DEBUG("is_mdt() returned false in segm-based mdt eff calc" );
        }
        //  std::string hardware_name = convertChamberName(m_mdtIdHelper->stationName(station_id), m_mdtIdHelper->stationEta(station_id), m_mdtIdHelper->stationPhi(station_id),type);
        std::string hardware_name = getChamberName(station_id); 

        // SEGMENT track
        const MuonGM::MdtReadoutElement* detEl = p_MuonDetectorManager->getMdtReadoutElement(station_id);
        Amg::Transform3D gToStation = detEl->GlobalToAmdbLRSTransform();
        Amg::Vector3D  segPosG(segment->globalPosition());
        Amg::Vector3D  segDirG(segment->globalDirection());
        Amg::Vector3D  segPosL  = gToStation*segPosG;
        Amg::Vector3D  segDirL = gToStation.linear()*segDirG;
        MuonCalib::MTStraightLine segment_track = MuonCalib::MTStraightLine(segPosL, segDirL, Amg::Vector3D(0,0,0), Amg::Vector3D(0,0,0));

        // Loop over tubes in chamber, find those along segment
        for(unsigned i_ML=0; i_ML<unique_chambers_ML.at(i_chamber).size(); i_ML++) {
          int ML = unique_chambers_ML.at(i_chamber).at(i_ML);
          Identifier newId = m_mdtIdHelper->channelID(hardware_name.substr(0,3), m_mdtIdHelper->stationEta(station_id), m_mdtIdHelper->stationPhi(station_id), ML, 1, 1);   
          int tubeMax = m_mdtIdHelper->tubeMax(newId);
          int tubeLayerMax = m_mdtIdHelper->tubeLayerMax(newId);
          CorrectTubeMax(hardware_name, tubeMax);
          CorrectLayerMax(hardware_name, tubeLayerMax);
          for(int i_tube=m_mdtIdHelper->tubeMin(newId); i_tube<=tubeMax; i_tube++) {
            for(int i_layer=m_mdtIdHelper->tubeLayerMin(newId); i_layer<=tubeLayerMax; i_layer++) {
              const MuonGM::MdtReadoutElement* MdtRoEl = p_MuonDetectorManager->getMdtReadoutElement( newId );
              if(m_BMGpresent && m_mdtIdHelper->stationName(newId) == m_BMGid ) {
                std::map<Identifier, std::vector<Identifier> >::iterator myIt = m_DeadChannels.find(MdtRoEl->identify());
                if( myIt != m_DeadChannels.end() ){
                  Identifier tubeId = m_mdtIdHelper->channelID(hardware_name.substr(0,3), m_mdtIdHelper->stationEta(station_id), m_mdtIdHelper->stationPhi(station_id), ML, i_layer, i_tube );
                  if( std::find( (myIt->second).begin(), (myIt->second).end(), tubeId) != (myIt->second).end() ) {
                    ATH_MSG_DEBUG("Skipping tube with identifier " << m_mdtIdHelper->show_to_string(tubeId) );
                    continue;
                  }
                }
              }
              Amg::Vector3D TubePos = MdtRoEl->GlobalToAmdbLRSCoords( MdtRoEl->tubePos( ML, i_layer, i_tube ) );
              Amg::Vector3D tube_position  = Amg::Vector3D(TubePos.x(), TubePos.y(), TubePos.z());
              Amg::Vector3D tube_direction = Amg::Vector3D(1,0,0);  
              MuonCalib::MTStraightLine tube_track = MuonCalib::MTStraightLine( tube_position, tube_direction, Amg::Vector3D(0,0,0), Amg::Vector3D(0,0,0));
              double distance = TMath::Abs(segment_track.signDistFrom(tube_track));
              if ( distance < (MdtRoEl->innerTubeRadius()) ){
                traversed_station_id.push_back(station_id);
                traversed_tube.push_back(i_tube);
                traversed_L.push_back(i_layer);
                traversed_ML.push_back(ML);
                traversed_distance.push_back( segment_track.signDistFrom(tube_track) );
              }      
            }
          }
        } 
      }
      // Done looping over the unqiue chambers

      // Loop over traversed tubes that were stored above
      // Here we fill the DRvsDT/DRvsSegD histos, as well is unique hits and traversed tubes to calculate efficiencies
      if(traversed_tube.size() < 20) { // quality cut here -- 20 traversed tubes is ridiculous and generates low efficiencies (these are due to non-pointing segments)
        for (unsigned k=0; k<traversed_tube.size(); k++) {
          //    std::string hardware_name = convertChamberName(m_mdtIdHelper->stationName(traversed_station_id.at(k)), m_mdtIdHelper->stationEta(traversed_station_id.at(k)), m_mdtIdHelper->stationPhi(traversed_station_id.at(k)),type);
          std::string hardware_name = getChamberName(traversed_station_id.at(k));
          // GET HISTS
          IdentifierHash idHash;
          m_mdtIdHelper->get_module_hash( traversed_station_id.at(k), idHash );
          MDTChamber* chamber;
          sc = getChamber( idHash, chamber );
          if(!sc.isSuccess()){
            ATH_MSG_ERROR( "Could Not Retrieve MDTChamber w/ ID " << idHash );
            return sc;
          }

          bool hit_flag = false;  
          for (unsigned j=0; j<ROTs_tube.size(); j++) {
            //      if( (convertChamberName(m_mdtIdHelper->stationName(ROTs_chamber.at(j)), m_mdtIdHelper->stationEta(ROTs_chamber.at(j)), m_mdtIdHelper->stationPhi(ROTs_chamber.at(j)),type) == hardware_name) 
            //    && (traversed_tube.at(k)==ROTs_tube.at(j)) && (traversed_L.at(k)==ROTs_L.at(j)) && (traversed_ML.at(k)==ROTs_ML.at(j))) { // found traversed tube with hit used in segment 
            if( (getChamberName(ROTs_chamber.at(j)) == hardware_name)
                && (traversed_tube.at(k)==ROTs_tube.at(j)) && (traversed_L.at(k)==ROTs_L.at(j)) && (traversed_ML.at(k)==ROTs_ML.at(j))) { // found traversed tube with hit used in segment 
              hit_flag = true;
              if(chamber->mdt_DRvsDT)   chamber->mdt_DRvsDT->Fill(ROTs_DR.at(j), ROTs_DT.at(j));
              if(chamber->mdt_DRvsDRerr) chamber->mdt_DRvsDRerr->Fill(ROTs_DR.at(j), ROTs_DRerr.at(j)); // plot appears useless
              if(chamber->mdt_DRvsSegD) chamber->mdt_DRvsSegD->Fill(ROTs_DR.at(j), traversed_distance.at(k));
              break;
            }
          }
          Identifier newId = m_mdtIdHelper->channelID(hardware_name.substr(0,3), m_mdtIdHelper->stationEta(traversed_station_id.at(k)), m_mdtIdHelper->stationPhi(traversed_station_id.at(k)), traversed_ML.at(k), 1, 1);
          int tubeLayerMax = m_mdtIdHelper->tubeLayerMax(newId);
          m_mdtIdHelper->get_module_hash( newId, idHash );

          CorrectLayerMax(hardware_name, tubeLayerMax); // ChamberTubeNumberCorrection handles the tubeMax problem
          int mdtlayer = ( (traversed_L.at(k) - 1) + (traversed_ML.at(k) - 1) * tubeLayerMax );
          int ibin = traversed_tube.at(k) + mdtlayer * m_mdtIdHelper->tubeMax(newId);
          ChamberTubeNumberCorrection(ibin, hardware_name, traversed_tube.at(k), mdtlayer);
          // Store info for eff calc
          // (Here we make sure we are removing duplicates from overlapping segments by using sets)
          std::set<TubeTraversedBySegment, TubeTraversedBySegment_cmp>::iterator it;
          TubeTraversedBySegment tmp_effTube = TubeTraversedBySegment(hardware_name, ibin, hit_flag, idHash);
          TubeTraversedBySegment tmp_effTube_Hit = TubeTraversedBySegment(hardware_name, ibin, true, idHash);
          TubeTraversedBySegment tmp_effTube_noHit = TubeTraversedBySegment(hardware_name, ibin, false, idHash);
          it = store_effTubes.find(tmp_effTube_Hit);
          if(  hit_flag || (it==store_effTubes.end()) ) store_effTubes.insert( tmp_effTube ); // Insert if w/hit, but if w/o hit then only insert if no already stored w/ hit
          it = store_effTubes.find(tmp_effTube_noHit);
          if(  hit_flag && (it!=store_effTubes.end()) ) store_effTubes.erase( it ); // If w/ hit, and the same tube is stored w/o hit, remove duplicate w/o hit
        }
      }
      // Done looping over traversed tubes

    }//m_doChamberHists
  }

  // Fill effentries/effcounts hists for efficiency calculation
  if(m_doChamberHists) { //Don't perform this block if not doing chamber by chamber hists
    for (std::set<TubeTraversedBySegment, TubeTraversedBySegment_cmp>::iterator it=store_effTubes.begin(); it!=store_effTubes.end(); it++) {
      // GET HISTS
      MDTChamber* chamber;
      sc = getChamber( (*it).idHash, chamber );
      if(!sc.isSuccess()){
        ATH_MSG_ERROR( "Could Not Retrieve MDTChamber w/ ID " << (*it).idHash );
        return sc;
      }
      if(chamber->mdt_effEntries) chamber->mdt_effEntries->Fill( (*it).tubeBin );
      if ( (*it).isHit && chamber->mdt_effCounts ) {
    	  
    	  chamber->mdt_effCounts->Fill( (*it).tubeBin );
      }      
    }
  }

  return sc; 
}

void MdtRawDataValAlg::initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl) {
  PVConstLink cv = mydetEl->getMaterialGeom(); // it is "Multilayer"
  int nGrandchildren = cv->getNChildVols();
  if(nGrandchildren <= 0) return;

  Identifier detElId = mydetEl->identify();

  int name = m_mdtIdHelper->stationName(detElId);
  int eta = m_mdtIdHelper->stationEta(detElId);
  int phi = m_mdtIdHelper->stationPhi(detElId);
  int ml = m_mdtIdHelper->multilayer(detElId);
  std::vector<Identifier> deadTubes;

  for(int layer = 1; layer <= mydetEl->getNLayers(); layer++){
    for(int tube = 1; tube <= mydetEl->getNtubesperlayer(); tube++){
      bool tubefound = false;
      for(unsigned int kk=0; kk < cv->getNChildVols(); kk++) {
        int tubegeo = cv->getIdOfChildVol(kk) % 100;
        int layergeo = ( cv->getIdOfChildVol(kk) - tubegeo ) / 100;
        if( tubegeo == tube && layergeo == layer ) {
          tubefound=true;
          break;
        }
        if( layergeo > layer ) break; // don't loop any longer if you cannot find tube anyway anymore
      }
      if(!tubefound) {
        Identifier deadTubeId = m_mdtIdHelper->channelID( name, eta, phi, ml, layer, tube );
        deadTubes.push_back( deadTubeId );
        ATH_MSG_VERBOSE("adding dead tube (" << tube  << "), layer(" <<  layer
                        << "), phi(" << phi << "), eta(" << eta << "), name(" << name
                        << "), multilayerId(" << ml << ") and identifier " << deadTubeId <<" .");
      }
    }
  }
  std::sort(deadTubes.begin(), deadTubes.end());
  m_DeadChannels[detElId] = deadTubes;
  return;
}
