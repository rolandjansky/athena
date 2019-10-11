
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtRawDataMonAlg
// Authors:   N. Benekos(Illinois) 
//            A. Cortes (Illinois) 
//            G. Dedes (MPI) 
//            Orin Harris (University of Washington)
//            Justin Griffiths (University of Washington)
//            M. Biglietti (INFN - Roma Tre)
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
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MdtCalibFitters/MTStraightLine.h"
#include "MuonSegment/MuonSegment.h"

#include "MdtRawDataMonitoring/MuonChamberIDSelector.h"
#include "MdtRawDataMonitoring/MdtRawDataMonAlg.h"
#include "TrkEventPrimitives/FitQuality.h"
//#include "xAODEventInfo/EventInfo.h"
//#include "xAODMuon/MuonContainer.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
// #include "GaudiKernel/Property.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
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

//float parESD1, parESD2, parESD3, parESD4;

//enum {enumBarrelA, enumBarrelC, enumEndCapA, enumEndCapC};
//enum {enumBarrel, enumEndCap};
enum {enumInner, enumMiddle, enumOuter, enumExtra};

/////////////////////////////////////////////////////////////////////////////
// *********************************************************************
// Public Methods
// ********************************************************************* 

MdtRawDataMonAlg::MdtRawDataMonAlg( const std::string& name, ISvcLocator* pSvcLocator )
 :AthMonitorAlgorithm(name,pSvcLocator),
 m_masked_tubes(NULL),
 m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
 m_DQFilterTools(this),
 m_atlas_ready(0),
  // m_trig_BARREL(false),
  // m_trig_ENDCAP(false),
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
  declareProperty("ADCCut",                  m_ADCCut=50.);
  declareProperty("ADCCutForBackground",     m_ADCCut_Bkgrd=80.);
  declareProperty("Eff_nHits",               m_nb_hits=5.);
  declareProperty("Eff_chi2Cut",             m_chi2_cut=10.);
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
  declareProperty("do_mdttube_masked",       m_do_mdttube_masked = true);
  declareProperty("do_mdttube_fornoise",     m_do_mdttube_fornoise = true);
  declareProperty("do_mdttube_bkgrd",        m_do_mdttube_bkgrd = true);
  declareProperty("do_mdtmezz",              m_do_mdtmezz = true);
  declareProperty("do_mdt_effEntries",       m_do_mdt_effEntries = true);
  declareProperty("do_mdt_effCounts",        m_do_mdt_effCounts = true);
  declareProperty("do_mdt_effPerTube",       m_do_mdt_effPerTube = false);
  declareProperty("do_mdt_DRvsDT",           m_do_mdt_DRvsDT = true);
  declareProperty("do_mdt_DRvsDRerr",        m_do_mdt_DRvsDRerr = false);
  declareProperty("do_mdt_DRvsSegD",         m_do_mdt_DRvsSegD = true);
  //declareProperty("nHits_NoiseThreshold",    m_HighOccThreshold = 16000.);
 declareProperty("nHits_NoiseThreshold",    m_HighOccThreshold = 400.);//DEV to-be-fixed
}

MdtRawDataMonAlg::~MdtRawDataMonAlg()
{
  // fixes for Memory leak
  if(m_hist_hash_list) {
    clear_hist_map(0);
  }
  delete m_masked_tubes; m_masked_tubes = NULL;
  ATH_MSG_DEBUG(" deleting MdtRawDataMonAlg " );

}

/*---------------------------------------------------------*/
StatusCode MdtRawDataMonAlg::initialize()
/*---------------------------------------------------------*/
{

  //initialize to stop coverity bugs
   p_MuonDetectorManager=0;
   //mdtevents_RPCtrig = 0;
   //mdtevents_TGCtrig=0;
   //   m_time = 0;
   m_firstTime = 0;
   
  
  // init message stream
  ATH_MSG_DEBUG("initialize MdtRawDataMonAlg" );

  ATH_MSG_DEBUG("******************" );
  ATH_MSG_DEBUG("doMdtESD: " << m_doMdtESD );
  ATH_MSG_DEBUG("******************" );

  StatusCode sc;

  //If Online ensure that lowStat histograms are made at the runLevel and that _lowStat suffix is suppressed

  //If online monitoring turn off chamber by chamber hists
  if(m_isOnline) m_doChamberHists = false;

  std::string managerName="Muon";
  sc = detStore()->retrieve(p_MuonDetectorManager);
  if (sc.isFailure()) {
    ATH_MSG_INFO("Could not find the MuonGeoModel Manager: " << managerName << " ! " );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the MuonGeoModel Manager " );
  }

  sc = m_muonIdHelperTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Cannot get MuonIdHelperTool" );
    return StatusCode::FAILURE;
  }  
  else {
    ATH_MSG_DEBUG(" Found the MuonIdHelperTool. " );
  }
  
  sc = m_DQFilterTools.retrieve();
  if( !sc ) {
    ATH_MSG_ERROR("Could Not Retrieve AtlasFilterTool " << m_DQFilterTools);
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_muonSelectionTool.retrieve());

  if(m_maskNoisyTubes) m_masked_tubes = new MDTNoisyTubes();
  else m_masked_tubes = new MDTNoisyTubes(false);
  mdtchamberId();

  ATH_CHECK(m_l1RoiKey.initialize());
  ATH_CHECK(m_muonKey.initialize());
  ATH_CHECK(m_segm_type.initialize());
  ATH_CHECK(m_key_mdt.initialize());
  ATH_CHECK(m_key_rpc.initialize());
  ATH_CHECK(m_eventInfo.initialize());

  m_BMGpresent = m_muonIdHelperTool->mdtIdHelper().stationNameIndex("BMG") != -1;
  //  m_BMGpresent = m_mdtIdHelper->stationNameIndex("BMG") != -1;
  if(m_BMGpresent){
    ATH_MSG_DEBUG("Processing configuration for layouts with BMG chambers.");
    //conflict 
    // m_BMGid = m_mdtIdHelper->stationNameIndex("BMG");
    m_BMGid = m_muonIdHelperTool->mdtIdHelper().stationNameIndex("BMG");
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

  std::string MDTHits_BE[2] = {"Barrel", "EndCap"};
  std::string ecap[4]={"BA","BC","EA","EC"};
  std::string layer[4]={"Inner","Middle","Outer","Extra"};

  for (int iecap=0;iecap<4;iecap++) {//BA,BC,EA,EC 
    for (int ilayer=0;ilayer<4;ilayer++) { //inner, middle, outer, extra
      	std::string s = "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut";
	m_mdthitspermultilayerLumi[iecap][ilayer] = new TH2F(s.c_str(), s.c_str(), 1, 0, 1, 1, 0, 1);
	string xAxis = ecap[iecap].substr(0,1) + layer[ilayer].substr(0,1) + ecap[iecap].substr(1,1);
	sc=binMdtRegional(m_mdthitspermultilayerLumi[iecap][ilayer], xAxis);

      if( ilayer==0 && ((iecap==0||iecap==2)) ) {
        s = "NumberOfHits"+MDTHits_BE[iecap/2];
	m_mdthitsperchamber_InnerMiddleOuterLumi[iecap/2] = new TH2F(s.c_str(), s.c_str(), 1, 0, 1, 1, 0, 1);  
	sc=binMdtGlobal(m_mdthitsperchamber_InnerMiddleOuterLumi[iecap/2], MDTHits_BE[iecap/2].at(0) );
      }
    }
  }

  std::string s = "NumberOfHitsInMDTInner_ADCCut";
  m_mdthitsperML_byLayer[enumInner] = new TH2F(s.c_str(), s.c_str(), 1, 0, 1, 1, 0, 1);
  s = "NumberOfHitsInMDTMIddle_ADCCut";
  m_mdthitsperML_byLayer[enumMiddle] = new TH2F(s.c_str(), s.c_str(), 1, 0, 1, 1, 0, 1);
  s = "NumberOfHitsInMDTOuter_ADCCut";
  m_mdthitsperML_byLayer[enumOuter] = new TH2F(s.c_str(), s.c_str(), 1, 0, 1, 1, 0, 1);
  sc = binMdtGlobal_byLayer(m_mdthitsperML_byLayer[enumInner], m_mdthitsperML_byLayer[enumMiddle], m_mdthitsperML_byLayer[enumOuter]);
  
  /*
  sc = bookMDTHisto_overview_2D(m_mdthitsperML_byLayer[enumInner], "NumberOfHitsInMDTInner_ADCCut","#eta station","#phi station", 
				1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
  sc = bookMDTHisto_overview_2D(m_mdthitsperML_byLayer[enumMiddle], "NumberOfHitsInMDTMiddle_ADCCut","#eta station","#phi station", 
				1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
  sc = bookMDTHisto_overview_2D(m_mdthitsperML_byLayer[enumOuter], "NumberOfHitsInMDTOuter_ADCCut","#eta station","#phi station", 
				1, 0, 1, 1, 0, 1, m_mg->mongroup_overview_shiftLumi);
  sc = binMdtGlobal_byLayer(m_mdthitsperML_byLayer[enumInner], m_mdthitsperML_byLayer[enumMiddle], m_mdthitsperML_byLayer[enumOuter]);
  */

  	//sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
	//						   "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
	//						   "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_brA_shiftLumi);      

  //  else if(iecap==enumBarrelC) sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
  //							    "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
  //							    "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_brC_shiftLumi);
  //  else if(iecap==enumEndCapA) sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
  //							    "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
  //							    "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_ecA_shiftLumi);
  //  else sc = bookMDTHisto_overview_2D(m_mdthitspermultilayerLumi[iecap][ilayer], 
  //				     "NumberOfHitsIn"+ecap[iecap]+layer[ilayer]+"PerMultiLayer_ADCCut",
  //				     "[Eta]", "[Phi,Multilayer]",1,0,1,1,0,1,m_mg->mongroup_ecC_shiftLumi);

  //DEV this before was in bookHistogramsRecurrent-- problably still needed since we need to fill m_hist_hash_list??  
  clear_hist_map();
  int counter = 0;

  for(vector<Identifier>::const_iterator itr = m_chambersId.begin(); itr != m_chambersId.end(); ++itr, ++counter){
    std::string hardware_name = convertChamberName(m_muonIdHelperTool->mdtIdHelper().stationName(*itr),m_muonIdHelperTool->mdtIdHelper().stationEta(*itr),
						   m_muonIdHelperTool->mdtIdHelper().stationPhi(*itr),"MDT");
    //Skip Chambers That Do NOT Exist
    if(hardware_name=="BML6A13" || hardware_name=="BML6C13") continue;
    
    MDTChamber* chamber = new MDTChamber(hardware_name);
    (*m_hist_hash_list)[m_chambersIdHash.at(counter)] = chamber;
   
    chamber->SetMDTHitsPerChamber_IMO_Bin(dynamic_cast<TH2F*> (m_mdthitsperchamber_InnerMiddleOuterLumi[chamber->GetBarrelEndcapEnum()]));
    chamber->SetMDTHitsPerML_byLayer_Bins(dynamic_cast<TH2F*> (m_mdthitspermultilayerLumi[chamber->GetRegionEnum()][chamber->GetLayerEnum()])
					  ,dynamic_cast<TH2F*> (m_mdthitsperML_byLayer[ (chamber->GetLayerEnum() < 3 ? chamber->GetLayerEnum() : 0) ]));
    /*DEV this was in bookHistogramsRecurrent, need to be reimplemented in someway    
    // chamber->SetMDTHitsPerChamber_IMO_Bin(dynamic_cast<TH2F*> (m_mdthitsperchamber_InnerMiddleOuter_HighOcc[chamber->GetBarrelEndcapEnum()]));
    
    // if(m_doChamberHists){
    //      sc = bookMDTHistograms(chamber, *itr); 
    //    }
    DEV */
  }

  //code to obtain the map of the numbers of tubes per chamber
  /*
  ofstream myfile;
  myfile.open ("tubeMaxMap.txt");
  for(vector<Identifier>::const_iterator itr = m_chambersId.begin(); itr != m_chambersId.end(); ++itr){
    std::string hardware_name = getChamberName( *itr );
    int tubeIdMax = GetTubeMax(*itr, hardware_name);
    std::cout << "\""<< hardware_name  << "\":\""<< tubeIdMax <<"\","<< std::endl; 
    myfile << "\""<< hardware_name  << "\":\""<< tubeIdMax <<"\",\n"; 
  }
  myfile.close();
  */
  
  ATH_MSG_DEBUG(" end of initialize " );
  return AthMonitorAlgorithm::initialize();
}


/*----------------------------------------------------------------------------------*/
StatusCode MdtRawDataMonAlg::fillHistograms(const EventContext& ctx) const
/*----------------------------------------------------------------------------------*/ 
{  
  StatusCode sc = StatusCode::SUCCESS;

  //Set ATLASReadyFlag
  //  setIsATLASReady(); //DEV still needed ? does not compile....


  //Get lumiblock, event number, timing info
  /* DEV still needed does not compile

  sc= GetTimingInfo(); //time is done below 
  if (sc.isFailure()){
       ATH_MSG_ERROR(" Cannot GetTimingInfo " );
       return sc;
  }
  DEV */

  int lumiblock = -1;
  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfo);
  lumiblock = evt->lumiBlock();
  
  uint32_t time;
  time = (1000000000L*(uint64_t)evt->timeStamp()+evt->timeStampNSOffset()) * 0.000000001;

  ATH_MSG_DEBUG("MdtRawDataMonAlg::MDT RawData Monitoring Histograms being filled" );

  // Retrieve the LVL1 Muon RoIs:
  bool trig_BARREL = false;
  bool trig_ENDCAP = false;
  try{
    SG::ReadHandle<xAOD::MuonRoIContainer> muonRoIs(m_l1RoiKey, ctx);
    if(!muonRoIs.isValid()){
      ATH_MSG_ERROR("evtStore() does not contain muon L1 ROI Collection with name "<< m_l1RoiKey);
      return StatusCode::FAILURE;
    }
    //DEV still needed ? does not compile
    if(muonRoIs.isPresent() && muonRoIs.isValid()){
      ATH_MSG_VERBOSE( "Retrieved LVL1MuonRoIs object with key: " << m_l1RoiKey.key() ); 
      xAOD::MuonRoIContainer::const_iterator mu_it = muonRoIs->begin(); 
      xAOD::MuonRoIContainer::const_iterator mu_it_end= muonRoIs->end();
      
      for( ; mu_it != mu_it_end; mu_it++){
	//ATH_MSG_ERROR( "(*mu_it)->getSource(): " << (*mu_it)->getSource() << ", is Muon_ROI::Endcap: " << ((*mu_it)->getSource()==(xAOD::MuonRoI::RoISource::Endcap)) << ", is Muon_ROI::Barrel: " << ((*mu_it)->getSource()==(xAOD::MuonRoI::RoISource::Barrel)) );
	if( (*mu_it)->getSource() == xAOD::MuonRoI::RoISource::Barrel) {
	  trig_BARREL =true;
	  break;
	}
      }
      for( ; mu_it != mu_it_end; mu_it++){
	if( (*mu_it)->getSource() == xAOD::MuonRoI::RoISource::Endcap ) {
	  trig_ENDCAP = true;
	  break;
	}
      }
    }
  }     catch (SG::ExcNoAuxStore & excpt){
    ATH_MSG_INFO("SG::ExcNoAuxStore caught, "<<m_l1RoiKey.key()<<" not available.");
  }

  //declare MDT stuff 
  SG::ReadHandle<Muon::MdtPrepDataContainer> mdt_container(m_key_mdt, ctx);
  if(!mdt_container.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain mdt prd Collection with name "<< m_key_mdt);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("****** mdtContainer->size() : " << mdt_container->size());  

  int nColl = 0;        // Number of MDT chambers with hits
  int nColl_ADCCut = 0; // Number of MDT chambers with hits above ADC cut
  int nPrd = 0;         // Total number of MDT prd digits
  int nPrdcut = 0;      // Total number of MDT prd digits with a cut on ADC>50.

  //declare RPC stuff
  SG::ReadHandle<Muon::RpcPrepDataContainer> rpc_container(m_key_rpc, ctx);
  if(!rpc_container.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain rpc prd Collection with name "<< m_key_rpc);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("****** rpc->size() : " << rpc_container->size());

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
    //DEV this shouls be done in some other way, in AthenaMonManager there is
    // Gaudi::Property<std::string> m_environmentStr {this,"Environment","user"}; ///< Environment string pulled from the job option and converted to enum
    //per il momento commento...
    //    if(m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online) {
    if(true) { //DEV to be updated

      SG::ReadHandle<xAOD::MuonContainer> muons(m_muonKey, ctx);
      if(!muons.isValid()){
	ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_muonKey);
	return StatusCode::FAILURE;
      }
      
      for(const auto mu : *muons){
	if(! (mu->muonType() == xAOD::Muon::Combined)) continue;
	xAOD::Muon::Quality quality = m_muonSelectionTool->getQuality(*mu);
	if (!(quality <= xAOD::Muon::Medium)) continue;
	const   xAOD::TrackParticle* tp = mu->primaryTrackParticle();
	if(tp){
	  /*DEV commented - not working
	  const Trk::Track * trk= tp->track();
	  if(!trk){
	    continue;
	  }
	  */
	  uint8_t ntri_eta=0;
	  uint8_t n_phi=0; 
	  tp->summaryValue(ntri_eta, xAOD::numberOfTriggerEtaLayers); 
	  tp->summaryValue(n_phi, xAOD::numberOfPhiLayers); 
	  if(ntri_eta+n_phi==0) continue;
	  /*DEV commented - not working
          for (const Trk::MeasurementBase* hit : *trk->measurementsOnTrack()) {
	    const Trk::RIO_OnTrack* rot_from_track = dynamic_cast<const Trk::RIO_OnTrack*>(hit);
	    if(!rot_from_track) continue;
	    //              rot_from_track->dump(msg());
	    Identifier rotId = rot_from_track->identify();
	    if(!m_muonIdHelperTool->mdtIdHelper().is_mdt(rotId)) continue;
	    IdentifierHash mdt_idHash;
	    MDTChamber* mdt_chamber = 0;
	    m_muonIdHelperTool->mdtIdHelper().get_module_hash( rotId, mdt_idHash );
	    sc = getChamber(mdt_idHash, mdt_chamber);
	    std::string mdt_chambername = mdt_chamber->getName();
	    chambers_from_tracks.insert(mdt_chambername);
	  }
	  DEV */
	  
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

          sc = fillMDTOverviewHistograms(*mdtCollection, isNoiseBurstCandidate);
          if(sc.isSuccess()) { 
            ATH_MSG_DEBUG("Filled MDTOverviewHistograms" );
          }
          else {
            ATH_MSG_ERROR("Failed to fill MDTOverviewHistograms" );
            return sc;
          }
	  //=======================================================================
	  //=======================================================================
	  //=======================================================================

	  sc = fillMDTSummaryHistograms(*mdtCollection,  chambers_from_tracks, isNoiseBurstCandidate, lumiblock, trig_BARREL, trig_ENDCAP);
          if(sc.isSuccess()){
            ATH_MSG_DEBUG("Filled MDTSummaryHistograms " );
          }   else {
            ATH_MSG_ERROR("Failed to Fill MDTSummaryHistograms" ); 
            return sc; 
          }
	  //=======================================================================
	  //=======================================================================
	  //=======================================================================
	  //	  std::cout << "m_doChamberHists=" << m_doChamberHists << std::endl;
	  if(m_doChamberHists){
	    // if(isATLASReady()) 
	    sc = fillMDTHistograms(*mdtCollection);
            if(sc.isSuccess()) {
              ATH_MSG_DEBUG("Filled MDThistograms (chamber by chamber histos)" );
            } else {
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

	  std::map<std::string,float> hitsperchamber_map;
	  if( adc >m_ADCCut) {
	    map<string,float>::iterator iter_hitsperchamber = hitsperchamber_map.find(hardware_name);
            if ( iter_hitsperchamber == hitsperchamber_map.end() ) { 
              hitsperchamber_map.insert( make_pair( hardware_name, 1 ) );
            } 
            else {
             iter_hitsperchamber->second += 1;
            }                
          }       

        } // for loop over hits
        if( isHit_above_ADCCut ) 
          nColl_ADCCut++;
      } //loop in MdtPrepDataContainer
      int nHighOccChambers = 0;
      map<string,float>::iterator iterstat;
      std::map<std::string,float> tubesperchamber_map;
      for( iterstat = evnt_hitsperchamber_map.begin(); iterstat != evnt_hitsperchamber_map.end(); ++iterstat ) {
          map<string,float>::iterator iter_tubesperchamber = tubesperchamber_map.find(hardware_name);
          float nTubes = iter_tubesperchamber->second;
          float hits = iterstat->second;
          float occ = hits/nTubes;
          if ( occ > 0.1 ) nHighOccChambers++;
      }

      /* DEV 
  if (m_nummdtchamberswithHighOcc) m_nummdtchamberswithHighOcc->Fill(nHighOccChambers);
      else {ATH_MSG_DEBUG("m_nummdtchamberswithHighOcc not in hist list!" );}

      m_MdtNHitsvsRpcNHits->Fill(nPrd,Nhitsrpc);
      

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

  
        if (m_mdtglobalhitstime) m_mdtglobalhitstime->Fill(m_time - m_firstTime);
      

      // Number_of_Chambers_with_hits_per_event
      if (m_nummdtchamberswithhits) m_nummdtchamberswithhits->Fill(nColl);
      else {ATH_MSG_DEBUG("m_nummdtchamberswithhits not in hist list!" );}

      // Number_of_Chambers_with_hits_per_event_ADCCut
      if (m_nummdtchamberswithhits_ADCCut) m_nummdtchamberswithhits_ADCCut->Fill(nColl_ADCCut);
      else {ATH_MSG_DEBUG("m_nummdtchamberswithhits_ADCCut not in hist list!" );}
      end DEV */
    }  //m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD   
  } //m_doMdtESD==true

  /*DEV
  SG::ReadHandle<Trk::SegmentCollection> segms(m_segm_type, ctx);
  if(!segms.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain mdt segms Collection with name "<< m_segm_type);
    return StatusCode::FAILURE;
  }
  // DEV still needed ? doeas not compile
  //if(isATLA_Ready()) 

  sc=handleEvent_effCalc( segms.cptr()); //, mdt_container );    
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Couldn't handleEvent_effCalc " );
    return sc;
  }
  DEV */
  return sc;
} 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdtRawDataMonAlg::fillMDTOverviewHistograms( const Muon::MdtPrepData* mdtCollection, bool &isNoiseBurstCandidate ) const {
  StatusCode sc = StatusCode::SUCCESS;
  Identifier digcoll_id = (mdtCollection)->identify();

  std::string hardware_name = getChamberName( mdtCollection );
  bool isNoisy = m_masked_tubes->isNoisy( mdtCollection );

  const MuonGM::MdtReadoutElement* pReadoutElementMDT = p_MuonDetectorManager->getMdtReadoutElement(digcoll_id);
  const Amg::Vector3D mdtgPos = pReadoutElementMDT->tubePos(digcoll_id); //global position of the wire                  
  float mdt_tube_eta   = mdtgPos.eta();     

  float tdc = mdtCollection->tdc()*25.0/32.0;
  // Note: the BMG is digitized with 200ps which is not same as other MDT chambers with 25/32=781.25ps
  if(hardware_name.substr(0,3)=="BMG") tdc = mdtCollection->tdc() * 0.2;

  float adc = mdtCollection->adc();
  if(hardware_name.substr(0,3) == "BMG") adc /= 4.;


  if( adc>m_ADCCut ) {
    //barrel
    if(fabs(mdt_tube_eta)>0. && fabs(mdt_tube_eta)<0.9) {
      auto mdt_tube_x_barrel = Monitored::Scalar<float>("mdt_tube_x_barrel", mdtgPos.x());
      auto mdt_tube_y_barrel = Monitored::Scalar<float>("mdt_tube_y_barrel", mdtgPos.y());
      auto mdt_tube_z_barrel = Monitored::Scalar<float>("mdt_tube_z_barrel", mdtgPos.z());
      auto mdt_tube_perp_barrel = Monitored::Scalar<float>("mdt_tube_perp_barrel", mdtgPos.perp());
      fill("MdtMonitor",mdt_tube_z_barrel,mdt_tube_perp_barrel, mdt_tube_x_barrel,mdt_tube_y_barrel);
    }   
    //OverLap -->Fill MDT Global RZ and YX
    if(fabs(mdt_tube_eta)>0.9 && fabs(mdt_tube_eta)<1.2) {
      auto mdt_tube_x_ovl = Monitored::Scalar<float>("mdt_tube_x_ovl", mdtgPos.x());
      auto mdt_tube_y_ovl = Monitored::Scalar<float>("mdt_tube_y_ovl", mdtgPos.y());
      auto mdt_tube_z_ovl = Monitored::Scalar<float>("mdt_tube_z_ovl", mdtgPos.z());
      auto mdt_tube_perp_ovl = Monitored::Scalar<float>("mdt_tube_perp_ovl", mdtgPos.perp());
      fill("MdtMonitor",mdt_tube_z_ovl,mdt_tube_perp_ovl, mdt_tube_x_ovl,mdt_tube_y_ovl);
    }
    //EndCap -->Fill MDT Global RZ and YX
    if(fabs(mdt_tube_eta)>1.2 && fabs(mdt_tube_eta)<2.7){
      auto mdt_tube_x_endcap = Monitored::Scalar<float>("mdt_tube_x_endcap", mdtgPos.x());
      auto mdt_tube_y_endcap = Monitored::Scalar<float>("mdt_tube_y_endcap", mdtgPos.y());
      auto mdt_tube_z_endcap = Monitored::Scalar<float>("mdt_tube_z_endcap", mdtgPos.z());
      auto mdt_tube_perp_endcap = Monitored::Scalar<float>("mdt_tube_perp_endcap", mdtgPos.perp());
      fill("MdtMonitor",mdt_tube_z_endcap,mdt_tube_perp_endcap, mdt_tube_x_endcap,mdt_tube_y_endcap);

    }      
  }

  auto adc_mon_nosel = Monitored::Scalar<float>("adc_mon_nosel", adc);
  fill("MdtMonitor", adc_mon_nosel);
  if(!isNoisy && adc > 0){
    auto tdc_mon = Monitored::Scalar<float>("tdc_mon", tdc);
    auto adc_mon = Monitored::Scalar<float>("adc_mon", adc);
    fill("MdtMonitor", tdc_mon, adc_mon);
  }

  if(isNoiseBurstCandidate){
    auto tdc_mon_noiseBurst = Monitored::Scalar<float>("tdc_mon_noiseBurst", tdc);
    auto adc_mon_noiseBurst = Monitored::Scalar<float>("adc_mon_noiseBurst", adc);
    fill("MdtMonitor", tdc_mon_noiseBurst, adc_mon_noiseBurst);
    if(!isNoisy) {
      auto adc_mon_noiseBurst_notNoisy = Monitored::Scalar<float>("adc_mon_noiseBurst_notNoisy", adc);
      fill("MdtMonitor",adc_mon_noiseBurst_notNoisy);
    }
    if( adc > m_ADCCut) {
      auto tdc_mon_noiseBurst_adcCut = Monitored::Scalar<float>("tdc_mon_noiseBurst_adcCut", tdc);
      fill("MdtMonitor",tdc_mon_noiseBurst_adcCut);
    }
  }

  if(adc > m_ADCCut){
    auto tdc_mon_adcCut = Monitored::Scalar<float>("tdc_mon_adcCut", tdc);
    fill("MdtMonitor",tdc_mon_adcCut);
  }

  return sc;

}

StatusCode MdtRawDataMonAlg::fillMDTSummaryHistograms( const Muon::MdtPrepData* mdtCollection, std::set<std::string>  chambers_from_tracks, bool &isNoiseBurstCandidate, int lb, bool trig_barrel, bool trig_endcap ) const{

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

  std::string region[4]={"BA","BC","EA","EC"};
  std::string layer[4]={"Inner","Middle","Outer","Extra"};
  //  std::string slayer[4]={"inner","middle","outer","extra"};

  int ibarrel = chamber->GetBarrelEndcapEnum();
  int iregion = chamber->GetRegionEnum();
  int ilayer = chamber->GetLayerEnum();
  int icrate = chamber->GetCrate();
  //
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
  
  auto lb_mon = Monitored::Scalar<int>("lb_mon", lb);
  auto sector = Monitored::Scalar<int>("sector",stationPhi+iregion*16);
  auto stationEta = Monitored::Scalar<int>("stEta_"+region[iregion]+"_"+layer[ilayer]+"_phi"+std::to_string(stationPhi+1), chamber->GetStationEta()); 

  fill("MdtMonitor",lb_mon, sector);
  //  mdtoccvslb_summaryPerSector->Fill(lumiblock,  stationPhi+iregion*16  );
  //MDTBA/Overview/Hits
  // iregion = BA/BC/EA/EC --> 4
  // ilayer = //inner, middle, outer, extra --> 4 
  // stationPhi --> 16  ====> 256
  //std::string mon="MDTHits_ADCCut_"+region[iregion]+"_Mon_"+layer[ilayer]+"_Phi_"+std::to_string(stationPhi+1);;

  std::string MDT_regionGroup="MDT_regionGroup"+region[iregion] ;//MDTXX/Overview, 4 gruppi

  //  int mlayer_n = m_mdtIdHelper->multilayer(digcoll_id);
  int mlayer_n = m_muonIdHelperTool->mdtIdHelper().multilayer(digcoll_id);

  if(!isNoisy && adc > 0){
    auto adc_mon =  Monitored::Scalar<float>("adc_mon", adc); 
    auto tdc_mon =  Monitored::Scalar<float>("tdc_mon", tdc); 
    fill(MDT_regionGroup, adc_mon, tdc_mon);
      if(isNoiseBurstCandidate) {
	auto tdc_mon_nb2 =  Monitored::Scalar<float>("tdc_mon_nb2", tdc); 
	auto adc_mon_nb2 =  Monitored::Scalar<float>("adc_mon_nb2", adc); 
	fill(MDT_regionGroup, tdc_mon_nb2, adc_mon_nb2);
      }
  }

  if(!isNoisy){
    //    fill(MDT_regionGroup, adc_mon);
    if(isNoiseBurstCandidate){
      auto tdc_mon_nb1 =  Monitored::Scalar<float>("tdc_mon_nb1", tdc); 
      auto adc_mon_nb1 =  Monitored::Scalar<float>("adc_mon_nb1", adc); 
      fill(MDT_regionGroup, tdc_mon_nb1, adc_mon_nb1);
    }
  }
  
  if( adc >m_ADCCut && !isNoisy) {

    auto adc_mon_adccut =  Monitored::Scalar<float>("adc_mon_adccut", adc);
    auto tdc_mon_adccut =  Monitored::Scalar<float>("tdc_mon_adccut", tdc); 
    fill(MDT_regionGroup, stationEta, tdc_mon_adccut, adc_mon_adccut);

    
    int binx=chamber->GetMDTHitsPerChamber_IMO_BinX();
    if(iregion<2) binx=binx-9;
    else binx=binx-7;
    int biny=chamber->GetMDTHitsPerChamber_IMO_BinY();

    string group = "MdtMonitor";
    string varx = " ";
    string vary = " ";
    string varx_noise = " ";
    string vary_noise = " ";
    if(iregion<2){
      varx="x_mon_barrel";
      vary="y_mon_barrel";
      varx_noise="x_mon_barrel_noise";
      vary_noise="y_mon_barrel_noise";
    } else {
      varx="x_mon_endcap";
      vary="y_mon_endcap";
      varx_noise="x_mon_endcap_noise";
      vary_noise="y_mon_endcap_noise";
    }

    auto x_mon =  Monitored::Scalar<int>(varx, binx);
    auto y_mon =  Monitored::Scalar<int>(vary, biny-1);
    fill(group,x_mon,y_mon);
    if(isNoiseBurstCandidate){
      auto x_mon_noise =  Monitored::Scalar<int>(varx_noise, binx);
      auto y_mon_noise =  Monitored::Scalar<int>(vary_noise, biny-1);
      fill(group,x_mon_noise,y_mon_noise);
      auto tdc_mon_nb3 =  Monitored::Scalar<float>("tdc_mon_nb3", tdc); 
      fill(MDT_regionGroup, tdc_mon_nb3);
    }
    

    varx = "x_mon_"+region[iregion]+"_"+layer[ilayer];
    vary = "y_mon_"+region[iregion]+"_"+layer[ilayer];

    auto x_bin_perML =   Monitored::Scalar<int>(varx, chamber->GetMDTHitsPerML_Binx()-1);//get the right bin!!!! 
    int biny_ml=0;
    if(mlayer_n==1) biny_ml=chamber->GetMDTHitsPerML_m1_Biny();
    else if(mlayer_n==2) biny_ml=chamber->GetMDTHitsPerML_m2_Biny();
    auto y_bin_perML =   Monitored::Scalar<int>(vary, biny_ml-1);
    //    fill(group,x_bin_perML,y_bin_perML);
    fill(MDT_regionGroup,x_bin_perML,y_bin_perML);

    if(layer[ilayer]!="Extra"){
      varx="x_mon_"+layer[ilayer];
      vary="y_mon_"+layer[ilayer];
      auto bin_byLayer_x = Monitored::Scalar<int>(varx,chamber->GetMDTHitsPerML_byLayer_BinX()-1); 
      auto bin_byLayer_y = Monitored::Scalar<int>(vary,chamber->GetMDTHitsPerML_byLayer_BinY(mlayer_n)-1); 
      
      fill("MdtMonitor",bin_byLayer_x,bin_byLayer_y);
      }
    //    if( HasTrigBARREL() ) m_overalltdccutPRLumi_RPCtrig[iregion]->Fill(tdc);
    //    if( HasTrigENDCAP() ) m_overalltdccutPRLumi_TGCtrig[iregion]->Fill(tdc);

    if( trig_barrel ) {
      auto tdc_mon_rpc =  Monitored::Scalar<float>("tdc_mon_rpc", tdc);
      fill(MDT_regionGroup,tdc_mon_rpc);
    }
    if( trig_endcap ) {
      auto tdc_mon_tgc =  Monitored::Scalar<float>("tdc_mon_tdc", tdc);
      fill(MDT_regionGroup,tdc_mon_tgc);
    }

    //DEV to DO
    // Fill occupancy vs. Lumiblock
    //    if(ilayer != 3) m_mdtoccvslb[iregion][ilayer]->Fill(m_lumiblock,get_bin_for_LB_hist(iregion,ilayer,stationPhi,stationEta,isBIM));
    //    else m_mdtoccvslb[iregion][2]->Fill(m_lumiblock,get_bin_for_LB_hist(iregion,ilayer,stationPhi,stationEta,isBIM)); // Put extras in with outer

    //correct readout crate info for BEE,BIS7/8
    int crate_region = iregion;
    if(chambername.substr(0,3)=="BEE" || (chambername.substr(0,3) == "BIS" && (stationEta == 7 || stationEta == 8) )){
      if(iregion==0) crate_region=2;
      if(iregion==1) crate_region=3;
    }
    //DEV to do
    //use stationPhi+1 because that's the actual phi, not phi indexed from zero.
    //    m_mdtoccvslb_by_crate[crate_region][icrate-1]->Fill(m_lumiblock,get_bin_for_LB_crate_hist(crate_region,icrate,stationPhi+1,stationEta,chambername));

    if (is_on_track)    {
      //      m_mdtoccvslb_ontrack_by_crate[crate_region][icrate-1]->Fill(m_lumiblock,get_bin_for_LB_crate_hist(crate_region,icrate,stationPhi+1,stationEta,chambername));
    }

  }  

  return sc;
}


StatusCode MdtRawDataMonAlg::fillMDTHistograms( const Muon::MdtPrepData* mdtCollection ) const{
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
  //  
  
  //      //convert layer numbering from 1->4 to 1->8
  //      //check if we are in 2nd multilayer
  //      //then add 4 if large chamber, 3 if small chamber 
  int mdtlayer = m_muonIdHelperTool->mdtIdHelper().tubeLayer(digcoll_id);
  if (m_muonIdHelperTool->mdtIdHelper().multilayer(digcoll_id)==2) {
    if ( hardware_name.at(1) == 'I' && hardware_name.at(3) != '8' )
      mdtlayer += 4;
    else 
      mdtlayer += 3;
  }   

  int mdttube= m_muonIdHelperTool->mdtIdHelper().tube(digcoll_id) + (mdtlayer-1) * m_muonIdHelperTool->mdtIdHelper().tubeMax(digcoll_id);
  ChamberTubeNumberCorrection(mdttube, hardware_name, m_muonIdHelperTool->mdtIdHelper().tube(digcoll_id), mdtlayer-1);
  bool isNoisy = m_masked_tubes->isNoisy( mdtCollection );

  float tdc = mdtCollection->tdc()*25.0/32.0;
  // Note: the BMG is digitized with 200ps which is not same as other MDT chambers with 25/32=781.25ps
  if(hardware_name.substr(0,3)=="BMG") tdc = mdtCollection->tdc() * 0.2;
  float adc = mdtCollection->adc();
  if(hardware_name.substr(0,3) == "BMG") adc /= 4.;


  //<<<<<<< HEAD
  int iregion = chamber->GetRegionEnum();

  //  if (chamber->mdttdc) {//DEV not needed anymore???
  
  std::string monPerCh="MdtMonPerChamber";
  if(iregion==0) monPerCh+="BA";
  if(iregion==1) monPerCh+="BC";
  if(iregion==2) monPerCh+="EA";
  if(iregion==3) monPerCh+="EC";
  
  auto tdc_perch = Monitored::Scalar<float>("tdc_perch_"+hardware_name,tdc);
  auto adc_perch = Monitored::Scalar<float>("adc_perch_"+hardware_name, adc);
  fill(monPerCh, tdc_perch, adc_perch );
  //  fill(monPerCh, tdc_perch);
  
  int mdtMultLayer = m_muonIdHelperTool->mdtIdHelper().multilayer(digcoll_id);

  if (  adc >m_ADCCut && !isNoisy ) {
    if (mdtMultLayer==1) {
      auto tdc_perch_ml1 = Monitored::Scalar<float>("tdc_perch_ml1_adccut_"+hardware_name,tdc);
      fill(monPerCh, tdc_perch_ml1);
    }
    if (mdtMultLayer==2) {
      auto tdc_perch_ml2 = Monitored::Scalar<float>("tdc_perch_ml2_adccut_"+hardware_name,tdc);
      fill(monPerCh, tdc_perch_ml2);
    }
    auto layer_perch = Monitored::Scalar<int>("layer_perch_"+hardware_name, mdtlayer);
    fill(monPerCh, layer_perch );
  }
  

  if(adc>0) {
    //    fill(monPerCh,  Monitored::Scalar<float>("tdc_perch2d", tdc), Monitored::Scalar<float>("adc_perch2d", adc) );
  }

  int mezz = mezzmdt(digcoll_id);
  if (  adc >m_ADCCut  )     {
    auto tube_perch = Monitored::Scalar<int>("tube_perch_"+hardware_name, mdttube);
    fill(monPerCh, tube_perch );
    //    fill(monPerCh, Monitored::Scalar<int>("mezz_perch",mezz) );
  }

  //  if (chamber->mdtmezz) { if( adc > m_ADCCut) chamber->mdtmezz->Fill( mezzmdt( digcoll_id ) ); }

  return sc;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

/* DEV3

 StatusCode MdtRawDataMonAlg::procHistograms(/*bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun ) {

<<<<<<< HEAD
  if(endOfRunFlag()) {
=======
  int mlayer_n = m_muonIdHelperTool->mdtIdHelper().multilayer(digcoll_id);
>>>>>>> upstream/master

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

      //Fills counts per hardware chambers in phi slice  plot
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
}														  }
DEV */

StatusCode MdtRawDataMonAlg::handleEvent_effCalc(const Trk::SegmentCollection* segms) const {
  StatusCode sc=StatusCode::SUCCESS;
  std::string type="MDT";
  std::set<monAlg::TubeTraversedBySegment, monAlg::TubeTraversedBySegment_cmp> store_effTubes;
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
        m_muonIdHelperTool->mdtIdHelper().get_module_hash( tmpid, idHash );  
        sc = getChamber(idHash, chamber);
        std::string chambername = chamber->getName();
        float adc = mrot->prepRawData()->adc();
        if(chambername.substr(0,3)=="BMG") adc /= 4. ;
	/*DEV
        //if(m_overalladc_segm_Lumi) m_overalladc_segm_Lumi->Fill(adc);
	 DEV */
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
	      /* DEV
	      //hard cut to adc only to present this histogram
              if(m_overalladc_segm_PR_Lumi[iregion] && adc > 50.) m_overalladc_segm_PR_Lumi[iregion]->Fill(adc);        
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
<<<<<<< HEAD
          int mdtMultLayer = m_mdtIdHelper->multilayer(tmpid);
          
=======
          int mdtMultLayer = m_muonIdHelperTool->mdtIdHelper().multilayer(tmpid);
          //chamber->mdtadc_onSegm->Fill(mrot->prepRawData()->adc());
>>>>>>> upstream/master
  	  if(chamber->mdtadc_onSegm_ML1 && mdtMultLayer == 1){
        	  chamber->mdtadc_onSegm_ML1->Fill(adc); 
	  }
          if(chamber->mdtadc_onSegm_ML2&& mdtMultLayer == 2){
        	  chamber->mdtadc_onSegm_ML2->Fill(adc);       	  
	  }
		DEV */
        }

        // This information needs to be stored fully for each segment (for calculations below), so deal with these duplicates later
        // (otherwise we may not check a traversed ML for a differently pointing overlapping segment, for example)

        ROTs_chamber.push_back( tmpid );
        ROTs_ML.push_back( m_muonIdHelperTool->mdtIdHelper().multilayer(tmpid) );
        ROTs_L.push_back( m_muonIdHelperTool->mdtIdHelper().tubeLayer(tmpid) );
        ROTs_tube.push_back( m_muonIdHelperTool->mdtIdHelper().tube(tmpid) );
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
            //    if( convertChamberName(m_muonIdHelperTool->mdtIdHelper().stationName(ROTs_chamber.at(i)), m_muonIdHelperTool->mdtIdHelper().stationEta(ROTs_chamber.at(i)), m_muonIdHelperTool->mdtIdHelper().stationPhi(ROTs_chamber.at(i)),type)
            //        == convertChamberName(m_muonIdHelperTool->mdtIdHelper().stationName(unique_chambers.at(j)), m_muonIdHelperTool->mdtIdHelper().stationEta(unique_chambers.at(j)), m_muonIdHelperTool->mdtIdHelper().stationPhi(unique_chambers.at(j)),type) ) {
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
        if( !m_muonIdHelperTool->mdtIdHelper().is_mdt( station_id ) ) {
          ATH_MSG_DEBUG("is_mdt() returned false in segm-based mdt eff calc" );
        }
        //  std::string hardware_name = convertChamberName(m_muonIdHelperTool->mdtIdHelper().stationName(station_id), m_muonIdHelperTool->mdtIdHelper().stationEta(station_id), m_muonIdHelperTool->mdtIdHelper().stationPhi(station_id),type);
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
          Identifier newId = m_muonIdHelperTool->mdtIdHelper().channelID(hardware_name.substr(0,3), m_muonIdHelperTool->mdtIdHelper().stationEta(station_id), m_muonIdHelperTool->mdtIdHelper().stationPhi(station_id), ML, 1, 1);   
          int tubeMax = m_muonIdHelperTool->mdtIdHelper().tubeMax(newId);
          int tubeLayerMax = m_muonIdHelperTool->mdtIdHelper().tubeLayerMax(newId);
          CorrectTubeMax(hardware_name, tubeMax);
          CorrectLayerMax(hardware_name, tubeLayerMax);
          for(int i_tube=m_muonIdHelperTool->mdtIdHelper().tubeMin(newId); i_tube<=tubeMax; i_tube++) {
            for(int i_layer=m_muonIdHelperTool->mdtIdHelper().tubeLayerMin(newId); i_layer<=tubeLayerMax; i_layer++) {
              const MuonGM::MdtReadoutElement* MdtRoEl = p_MuonDetectorManager->getMdtReadoutElement( newId );
	      if(m_BMGpresent && m_muonIdHelperTool->mdtIdHelper().stationName(newId) == m_BMGid ) {
                std::map<Identifier, std::vector<Identifier> >::const_iterator myIt = m_DeadChannels.find(MdtRoEl->identify());
                if( myIt != m_DeadChannels.end() ){
                  Identifier tubeId = m_muonIdHelperTool->mdtIdHelper().channelID(hardware_name.substr(0,3), m_muonIdHelperTool->mdtIdHelper().stationEta(station_id), m_muonIdHelperTool->mdtIdHelper().stationPhi(station_id), ML, i_layer, i_tube );
                  if( std::find( (myIt->second).begin(), (myIt->second).end(), tubeId) != (myIt->second).end() ) {
                    ATH_MSG_DEBUG("Skipping tube with identifier " << m_muonIdHelperTool->mdtIdHelper().show_to_string(tubeId) );
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
          //    std::string hardware_name = convertChamberName(m_muonIdHelperTool->mdtIdHelper().stationName(traversed_station_id.at(k)), m_muonIdHelperTool->mdtIdHelper().stationEta(traversed_station_id.at(k)), m_muonIdHelperTool->mdtIdHelper().stationPhi(traversed_station_id.at(k)),type);
          std::string hardware_name = getChamberName(traversed_station_id.at(k));
          // GET HISTS
          IdentifierHash idHash;
          m_muonIdHelperTool->mdtIdHelper().get_module_hash( traversed_station_id.at(k), idHash );
          MDTChamber* chamber;
          sc = getChamber( idHash, chamber );
          if(!sc.isSuccess()){
            ATH_MSG_ERROR( "Could Not Retrieve MDTChamber w/ ID " << idHash );
            return sc;
          }

          bool hit_flag = false;  
          for (unsigned j=0; j<ROTs_tube.size(); j++) {
            //      if( (convertChamberName(m_muonIdHelperTool->mdtIdHelper().stationName(ROTs_chamber.at(j)), m_muonIdHelperTool->mdtIdHelper().stationEta(ROTs_chamber.at(j)), m_muonIdHelperTool->mdtIdHelper().stationPhi(ROTs_chamber.at(j)),type) == hardware_name) 
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
          Identifier newId = m_muonIdHelperTool->mdtIdHelper().channelID(hardware_name.substr(0,3), m_muonIdHelperTool->mdtIdHelper().stationEta(traversed_station_id.at(k)), m_muonIdHelperTool->mdtIdHelper().stationPhi(traversed_station_id.at(k)), traversed_ML.at(k), 1, 1);
          int tubeLayerMax = m_muonIdHelperTool->mdtIdHelper().tubeLayerMax(newId);
          m_muonIdHelperTool->mdtIdHelper().get_module_hash( newId, idHash );

          CorrectLayerMax(hardware_name, tubeLayerMax); // ChamberTubeNumberCorrection handles the tubeMax problem
          int mdtlayer = ( (traversed_L.at(k) - 1) + (traversed_ML.at(k) - 1) * tubeLayerMax );
          int ibin = traversed_tube.at(k) + mdtlayer * m_muonIdHelperTool->mdtIdHelper().tubeMax(newId);
          ChamberTubeNumberCorrection(ibin, hardware_name, traversed_tube.at(k), mdtlayer);
          // Store info for eff calc
          // (Here we make sure we are removing duplicates from overlapping segments by using sets)
          std::set<monAlg::TubeTraversedBySegment, monAlg::TubeTraversedBySegment_cmp>::iterator it;
	  monAlg::TubeTraversedBySegment tmp_effTube = monAlg::TubeTraversedBySegment(hardware_name, ibin, hit_flag, idHash);
	  monAlg::TubeTraversedBySegment tmp_effTube_Hit = monAlg::TubeTraversedBySegment(hardware_name, ibin, true, idHash);
	  monAlg::TubeTraversedBySegment tmp_effTube_noHit = monAlg::TubeTraversedBySegment(hardware_name, ibin, false, idHash);
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
    for (std::set<monAlg::TubeTraversedBySegment, monAlg::TubeTraversedBySegment_cmp>::iterator it=store_effTubes.begin(); it!=store_effTubes.end(); it++) {
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


void MdtRawDataMonAlg::initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl) {
  PVConstLink cv = mydetEl->getMaterialGeom(); // it is "Multilayer"
  int nGrandchildren = cv->getNChildVols();
  if(nGrandchildren <= 0) return;

  Identifier detElId = mydetEl->identify();

  int name = m_muonIdHelperTool->mdtIdHelper().stationName(detElId);
  int eta = m_muonIdHelperTool->mdtIdHelper().stationEta(detElId);
  int phi = m_muonIdHelperTool->mdtIdHelper().stationPhi(detElId);
  int ml = m_muonIdHelperTool->mdtIdHelper().multilayer(detElId);
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
        Identifier deadTubeId = m_muonIdHelperTool->mdtIdHelper().channelID( name, eta, phi, ml, layer, tube );
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



