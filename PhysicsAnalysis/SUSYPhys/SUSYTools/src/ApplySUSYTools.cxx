//////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation for ApplySUSYTools
// Author: Frank Paige <paige@bnl.gov>
///////////////////////////////////////////////////////////////////

#include "ApplySUSYTools.h"
#include "SUSYTools/SUSYObjDef_xAOD.h"
#include "AthenaKernel/IThinningSvc.h"

// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetTypes.h"
#include "xAODBTagging/BTagging.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODCore/ShallowCopy.h"
#include "AthContainers/AuxElement.h"
#include "AsgTools/AsgMetadataTool.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"

// Needed for systematics
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"
//#include "PATInterfaces/SystematicList.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PathResolver/PathResolver.h"

namespace ST {

//////////////
// Constructor
//////////////

ApplySUSYTools::ApplySUSYTools( const std::string& name,
                    ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_eleBaselinePt(0.),
  m_elePt(0.),
  m_eleEta(0.),
  m_eled0sig(0.),
  m_elez0(0.),
  m_muBaselinePt(0.),
  m_muPt(0.),
  m_muEta(0.),
  m_mud0sig(0.),
  m_muz0(0.),
  m_muCosmicz0(0.),
  m_muCosmicd0(0.),
  m_photonBaselinePt(0.),
  m_photonEta(0.),
  m_photonPt(0.),
  m_tauPt(0.),
  m_tauEta(0.),
  m_jetPt(0.), 
  m_jetEta(0.),
  m_jetJvt(0.),
  m_badjetJvt(0.),
  m_evtCount(0),
  m_maxCount(0),
  m_jetPtCut(20000.),
  m_inDetPtCut(1000.),
  m_doTST(true),
  m_isData(0),
  m_MuonsName("Muons"),
  m_MuonSpecTPName("MuonSpectrometerTrackParticles"),
  m_ElectronsName("Electrons"),
  m_JetsName("AntiKt4EMTopoJets"),
  m_FatJetsName("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"),
  m_BTagName("BTagging_AntiKt4EMTopo"),
  m_TruthJetsName("AntiKt4TruthJets"),
  m_METsName("MET_Reference_AntiKt4EMTopo"),
  m_InDetTracksName("InDetTrackParticles"),
  m_GSFTracksName("GSFTrackParticles"),
  m_GSFVerticesName("GSFConversionVertices"),
  m_TauJetsName("TauJets"),
  m_PhotonsName("Photons"),
  m_configFile("SUSYTools/SUSYTools_Default.conf"),
  m_objTool("SUSYObjDef_xAOD/dummy"),
  m_tauTruthTool("TauTruthMatchingTool/dummy"),
  m_thinningSvc("ThinningSvc/ThinningSvc", name)
{

  // Property declaration
  // Output name = input name + systematic name

  declareProperty("MuonsName", m_MuonsName);
  declareProperty("MuonSpecTracksName", m_MuonSpecTPName);
  declareProperty("ElectronsName", m_ElectronsName);
  declareProperty("JetsName", m_JetsName);
  declareProperty("FatJetsName", m_FatJetsName);
  declareProperty("BTaggingName", m_BTagName);
  declareProperty("TruthJetsName", m_TruthJetsName);
  declareProperty("METsName", m_METsName);
  declareProperty("JetsPtCut", m_jetPtCut);
  declareProperty("InDetTracksName", m_InDetTracksName);
  declareProperty("InDetPtCut", m_inDetPtCut);
  declareProperty("GSFTracksName", m_GSFTracksName);
  declareProperty("GSFVerticesName", m_GSFVerticesName);
  declareProperty("ElectronTriggers", m_ElectronTrigs);
  declareProperty("MuonTriggers", m_MuonTrigs);
  declareProperty("DoTST", m_doTST);
  declareProperty("TauJetsName", m_TauJetsName);
  declareProperty("PhotonsName", m_PhotonsName);
  declareProperty("PhotonTriggers", m_PhotonTrigs);

  declareProperty("IsData", m_isData);
  declareProperty("MaxCount", m_maxCount);
  declareProperty("SUSYObjTool",  m_objTool);
  declareProperty("TauTruthMatchingTool", m_tauTruthTool),
  declareProperty("ThinningSvc",m_thinningSvc);
  //declareProperty("", );
}

/////////////
// Destructor
/////////////

ApplySUSYTools::~ApplySUSYTools()
{}


////////////////////
// Athena initialize
////////////////////

StatusCode ApplySUSYTools::initialize()
{
  ATH_MSG_INFO ("ApplySUSYTools::initialize(): start");

  m_evtCount = -1;

  m_doFatJets=true;

  // Get pointer to ThinningSvc and cache it :
  // m_thinningSvc is of type IThinningSvc
  CHECK( m_thinningSvc.retrieve() );

  // SUSYTools: Configure in Python
  ATH_MSG_INFO("ApplySUSYTools::initialize(): retrieve m_objTool");
  CHECK( m_objTool.retrieve() );

  // isData() in SUSYObjDef_xAOD.h but not ISUSYObjDef_xAOD.h. 
  // Failed to retrieve EventInfo?!
  //const xAOD::EventInfo* eventInfo = 0;
  //CHECK( evtStore()->retrieve( eventInfo, "EventInfo") );
  //m_isData = true;
  //if( eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) m_isData = false;
  // Use jobOptions for now. :-(

  // Need truth matching for tau CP tools

  if( !m_isData ){
    ATH_MSG_INFO("ApplySUSYTools::initialize(): retrieve m_tauTruthTool");
    CHECK( m_tauTruthTool.retrieve() );
  }

  // Systematics
  // First entry is default; use only it for data
  m_systInfoList = m_objTool->getSystInfoList();
  ATH_MSG_INFO("Available systematics");
  for(const auto& si : m_systInfoList){
    std::string siname = si.systset.name();
    bool affKine = si.affectsKinematics;
    bool affWt = si.affectsWeights;
    unsigned int affTyp = si.affectsType;
    ATH_MSG_INFO("Systematic name/kine/wt/type \"" <<siname <<"\" " <<affKine <<" "
                 <<affWt <<" " <<affTyp);
    if( siname == "" ){
      m_systInfoJET.push_back(&si);
      m_systInfoELE.push_back(&si);
      m_systInfoMUON.push_back(&si);
      m_systInfoTAU.push_back(&si);
      m_systInfoPHO.push_back(&si);
      if( m_isData ) break;
    }
    if( siname.substr(0,4) == "JET_" ) m_systInfoJET.push_back(&si);
    if( siname.substr(0,4) == "JER_" ) m_systInfoJET.push_back(&si);
    if( siname.substr(0,3) == "EG_" ) m_systInfoELE.push_back(&si);
    if( siname.substr(0,3) == "EL_" ) m_systInfoELE.push_back(&si);
    if( siname.substr(0,4) == "MUON" ) m_systInfoMUON.push_back(&si);
    if( siname.substr(0,11) == "MET_SoftTrk" ) m_systInfoTST.push_back(&si);
    if( siname.substr(0,11) == "MET_SoftCal" ) m_systInfoCST.push_back(&si);
    if( siname.substr(0,5) == "TAUS_" ) m_systInfoTAU.push_back(&si);
    if( siname.substr(0,3) == "EG_" ) m_systInfoPHO.push_back(&si);
    if( siname.substr(0,3) == "PH_" ) m_systInfoPHO.push_back(&si);
  }

  // Mask names
  m_MuonCutMaskName = m_MuonsName + "CutMask";
  m_MuonSpecCutMaskName = m_MuonSpecTPName + "CutMask";
  m_ElectronCutMaskName = m_ElectronsName + "CutMask";
  m_JetCutMaskName = m_JetsName + "CutMask";
  m_TruthJetCutMaskName = m_TruthJetsName + "CutMask";
  m_InDetCutMaskName = m_InDetTracksName + "CutMask";
  m_GSFTrackCutMaskName = m_GSFTracksName + "CutMask";
  m_GSFVertexCutMaskName = m_GSFVerticesName + "CutMask";
  m_TauJetCutMaskName = m_TauJetsName  + "CutMask";
  m_PhotonCutMaskName = m_PhotonsName + "CutMask";

  ATH_MSG_INFO( "*****     *****     *****     *****" );
  ATH_MSG_INFO( "Configuring from file " << m_configFile );

  TEnv rEnv;
  int success = -1;
  m_configFile = (PathResolverFindCalibFile(m_configFile)).c_str();
  success = rEnv.ReadFile(m_configFile.c_str(), kEnvAll);
  if(success != 0){
    ATH_MSG_ERROR( "Cannot open config file!");
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "Config file opened" );

  // TauJet cuts set by tau_selection_*.conf config files

  m_eleBaselinePt = rEnv.GetValue("EleBaseline.Pt", 10000.);
  m_elePt = rEnv.GetValue("Ele.Et", 25000.);
  m_eleEta = rEnv.GetValue("Ele.Eta", 2.47);
  m_eled0sig = rEnv.GetValue("Ele.d0sig", 5.);
  m_elez0 = rEnv.GetValue("Ele.z0", 0.5);
  ATH_MSG_DEBUG("Config Electrons " <<m_eleBaselinePt <<" " <<m_elePt <<" "
                <<m_eleEta <<" " <<m_eled0sig <<" " <<m_elez0);

  m_muBaselinePt = rEnv.GetValue("MuonBaseline.Pt", 10000.);
  m_muPt = rEnv.GetValue("Muon.Pt", 25000.);
  m_muEta = rEnv.GetValue("Muon.Eta", 2.7);
  m_mud0sig = rEnv.GetValue("Muon.d0sig", 3.);
  m_muz0 = rEnv.GetValue("Muon.z0", 0.5);
  m_muCosmicz0 = rEnv.GetValue("MuonCosmic.z0", 1.);
  m_muCosmicd0 = rEnv.GetValue("MuonCosmic.d0", 0.2);
  ATH_MSG_DEBUG("Config Muons " <<m_muBaselinePt <<" " <<m_muPt <<" "
                <<m_muEta <<" " <<m_mud0sig <<" " <<m_muz0);

  m_photonBaselinePt = rEnv.GetValue("PhotonBaseline.Pt", 25000.);
  m_photonPt = rEnv.GetValue("Photon.Pt", 130000.);
  m_photonEta = rEnv.GetValue("Photon.Eta", 2.37);
  ATH_MSG_DEBUG("Config Photons " <<m_photonBaselinePt <<" " <<m_photonPt <<" " 
                <<m_photonEta);

  m_tauPt = rEnv.GetValue("Tau.Pt", 20000.);
  m_tauEta = rEnv.GetValue("Tau.Eta", 2.47);
  ATH_MSG_DEBUG("Config TauJets " <<m_tauPt <<" " <<m_tauEta);

  m_jetPt = rEnv.GetValue("Jet.Pt", 20000.);
  m_jetEta = rEnv.GetValue("Jet.Eta", 2.8);
  m_jetJvt = rEnv.GetValue("Jet.Jvt", 0.64);
  m_badjetJvt = rEnv.GetValue("BadJet.Jvt", 0.64);
  ATH_MSG_DEBUG("Config Jets " <<m_jetPt <<" " <<m_jetEta <<" "
                <<m_jetJvt <<" " <<m_badjetJvt);

  ATH_MSG_INFO("Config file done.");

  return StatusCode::SUCCESS;
}


//////////////////
// Athena finalize
//////////////////

StatusCode ApplySUSYTools::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}


/////////////////
// Athena execute
/////////////////

StatusCode ApplySUSYTools::execute()
{

  ++m_evtCount;
  bool doPrint = false;
  doPrint = m_evtCount < m_maxCount;

  if( m_evtCount%100 == 0 ){
    ATH_MSG_INFO ("Executing " <<name() <<" for event " <<m_evtCount);
  }
  if( doPrint ) ATH_MSG_INFO("Begin ApplySUSYTools for event " <<m_evtCount);

  // Reset systematics
  CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );


  ////////////////////////////
  // Retrieve original objects
  ////////////////////////////

  // Retrieve input containers. SUSYTools GetXXX can do this, but we
  // use FillXXX. Make ViewContainer for each systematic including
  // "Nominal".

  const xAOD::MuonContainer* p_Muons = 0;
  CHECK( evtStore()->retrieve(p_Muons, m_MuonsName) );
  if( doPrint ) ATH_MSG_DEBUG("Got p_Muons size =  " <<p_Muons->size());

  const xAOD::TrackParticleContainer* p_MuonSpecTP = 0;
  CHECK( evtStore()->retrieve(p_MuonSpecTP, m_MuonSpecTPName) );
  if( doPrint ) ATH_MSG_DEBUG("Got p_MuonSpecTP size =  " <<p_MuonSpecTP->size());

  const xAOD::ElectronContainer* p_Electrons = 0;
  CHECK( evtStore()->retrieve(p_Electrons, m_ElectronsName) );
  if( doPrint ) ATH_MSG_DEBUG("Got p_Electrons size = " <<p_Electrons->size());

  const xAOD::JetContainer* p_Jets = 0;
  CHECK( evtStore()->retrieve(p_Jets, m_JetsName) );
  if( doPrint ) ATH_MSG_DEBUG("Got p_Jets size =  " <<p_Jets->size());

  const xAOD::JetContainer* p_FatJets = 0;
  if( evtStore()->contains<xAOD::JetContainer>(m_FatJetsName) ){
    if( !evtStore()->retrieve( p_FatJets, m_FatJetsName).isSuccess() ){
      ATH_MSG_ERROR("Failed to retrieve xAOD::JetContainer with name "
                    << m_FatJetsName);
      return StatusCode::FAILURE;
    } else{
      if( doPrint ) ATH_MSG_DEBUG("Got p_FatJets size =  " 
                                  <<p_FatJets->size());
    }
  } else {
    if( doPrint ) ATH_MSG_DEBUG("FatJets xAOD::JetContainer with name "
                                << m_FatJetsName <<" not found");
    m_doFatJets=false;
  }

  const xAOD::BTaggingContainer* p_bTag = 0;
  CHECK( evtStore()->retrieve(p_bTag, m_BTagName) );
  if( doPrint ) ATH_MSG_DEBUG("Got p_bTag size =  " <<p_bTag->size());
  // Need to set this to avoid crashes in decorator functions.
  static SG::AuxElement::Decorator<char> dec_passOR("passOR");

  const xAOD::VertexContainer* p_Vertices = 0;
  CHECK( evtStore()->retrieve(p_Vertices, "PrimaryVertices") );
  const xAOD::Vertex* p_PrimVtx = m_objTool->GetPrimVtx();
  float z0PV = -666;
  if( p_PrimVtx ) z0PV = p_PrimVtx->z();
  if( doPrint ) ATH_MSG_DEBUG("Got p_Vertices size,z0 = " 
                              <<p_Vertices->size() <<" " <<z0PV);

  const xAOD::TauJetContainer* p_TauJets = 0;
  CHECK( evtStore()->retrieve(p_TauJets, m_TauJetsName) );
  if( doPrint ) ATH_MSG_DEBUG("Got p_TauJets size =  " <<p_TauJets->size());

  const xAOD::PhotonContainer* p_Photons = 0;
  CHECK( evtStore()->retrieve(p_Photons, m_PhotonsName) );
  if( doPrint ) ATH_MSG_DEBUG("Got p_Photons size =  " <<p_Photons->size());

  if( doPrint ) ATH_MSG_DEBUG("Retrieved all object containers for event " <<m_evtCount);

  // FIXME: Make empty TruthTaus if p_TauJets is empty and not data
  // Tau code fails in this case!
  if( !evtStore()->contains<xAOD::TruthParticleContainer>("TruthTaus") &&
  p_TauJets->size() == 0 && !m_isData ){
    ATH_MSG_DEBUG("FIXME: TruthTaus hack for empty TauJets.");
    xAOD::TruthParticleContainer* noTT = new xAOD::TruthParticleContainer;
    xAOD::TruthParticleAuxContainer* noTTAux = new xAOD::TruthParticleAuxContainer;
    noTT->setStore( noTTAux );
    CHECK( evtStore()->record(noTT, "TruthTaus") );
    CHECK( evtStore()->record(noTTAux, "TruthTausAux.") );
  }


  /////////////////////////////////////////////////////////
  // Make shallowCopy for each systematic including Nominal
  // Order in each matches that in original container.
  /////////////////////////////////////////////////////////

  ///////
  // Jets
  ///////

  // Initialize mask to false; true if any variation passes "baseline"
  // AND if pt > m_jetPtCut.
  std::vector<bool>* jetCutMask = new std::vector<bool>(p_Jets->size(),false);
  CHECK( evtStore()->record(jetCutMask, m_JetCutMaskName) );

  for(auto si : m_systInfoJET){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    if( doPrint ) ATH_MSG_DEBUG("Applying jet systematic " <<sysname);
    CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);

    // This largely replicates GetJets
    // setShallowIO writes output as shallow copy
    std::pair<xAOD::JetContainer*, xAOD::ShallowAuxContainer*> cpPair = 
    xAOD::shallowCopyContainer(*p_Jets);
    xAOD::JetContainer* cpJets = cpPair.first;
    xAOD::ShallowAuxContainer* cpAux = cpPair.second;    
    cpAux->setShallowIO( true );
    if( !xAOD::setOriginalObjectLink(*p_Jets, *cpJets) ){
      ATH_MSG_ERROR("Failed setOriginalObjectLink");
      return StatusCode::FAILURE;
    }
    CHECK( evtStore()->record(cpJets, m_JetsName+sysname) );
    CHECK( evtStore()->record(cpAux, m_JetsName+sysname+"Aux.") );
    if( doPrint ) ATH_MSG_DEBUG("Recorded jet shallow copy + aux for " 
                                <<m_JetsName+sysname);

    unsigned int pIndex = 0;
    for(const auto& p : *cpJets){
      CHECK( m_objTool->FillJet(*p, true) );
      //dec_passOR(*p) = true;
      bool isbad = m_objTool->IsBadJet(*p);
      bool issig = m_objTool->IsSignalJet(*p, m_jetPt, m_jetEta);
      bool isbjet = m_objTool->IsBJet(*p);
      if( doPrint ) ATH_MSG_DEBUG("Systematic jet pt " <<sysname <<" "
                                  <<p->pt() <<" " 
                                  <<(int)p->auxdata<char>("baseline")
                                  <<" " <<isbad <<" " <<issig <<" " <<isbjet);
      if( p->auxdata<char>("baseline") && p->pt() > m_jetPtCut ){ 
        (*jetCutMask)[pIndex] = true;
      }
      ++pIndex;
    }
  }

  // Jets thinning
  CHECK( m_thinningSvc->filter(*p_Jets, *jetCutMask, 
                               IThinningSvc::Operator::Or) );
  for(auto si : m_systInfoJET){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    const xAOD::JetContainer* jets = 0;
    CHECK( evtStore()->retrieve(jets, m_JetsName+sysname) );
    CHECK( m_thinningSvc->filter(*jets, *jetCutMask,
                                 IThinningSvc::Operator::Or) );
  }


  ///////
  // Fat Jets (nominal only!)
  ///////
  if(m_doFatJets){
    CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );
    
    std::string sysname = "Nominal";
    if( doPrint ) ATH_MSG_DEBUG("Applying jet systematic " <<sysname);
    
    // This largely replicates GetFatJets
    // setShallowIO writes output as shallow copy
    xAOD::JetContainer* fatjets_nominal(0);
    xAOD::ShallowAuxContainer* fatjets_nominal_aux(0);
    std::string fatjetcoll = m_FatJetsName.substr(0, m_FatJetsName.size()-4);
    CHECK( m_objTool->GetFatJets(fatjets_nominal, fatjets_nominal_aux, true, fatjetcoll, true));
    
    if( doPrint ){
      for (const auto& fatjet : *fatjets_nominal) {
	ATH_MSG_DEBUG("--------------------------------------");
	ATH_MSG_DEBUG("Recorded fat jet pt = " <<  fatjet->pt()*0.001 << " ,"
		      << " Wtag = " << (int)fatjet->auxdata<int>("Wtag") << " ,"
		      << " Ztag = " << (int)fatjet->auxdata<int>("Ztag"));
      }
    }
  }//endif doFatJets

  ////////
  // Muons
  ////////

  CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );

  // Initialize mask to false; true if any variation passes "baseline".
  // Pass m_muPtCut to FillMuon
  // Also keep MuonSpectrometerTrackParticles
  // InnerDetectorTrackParticles with all tracks below

  std::vector<bool>* muCutMask = new std::vector<bool>(p_Muons->size(),false);
  CHECK( evtStore()->record(muCutMask, m_MuonCutMaskName) );
  std::vector<bool>* muSpecCutMask = 0;
  if( p_MuonSpecTP ){
    muSpecCutMask = new std::vector<bool>(p_MuonSpecTP->size(),false);
    CHECK( evtStore()->record(muSpecCutMask, m_MuonSpecCutMaskName) );
  }

  for(auto si : m_systInfoMUON){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    if( doPrint ) ATH_MSG_DEBUG("Applying muon systematic " <<sysname);
    CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);

    // This largely replicates GetMuons
    // setShallowIO writes output as shallow copy

    std::pair<xAOD::MuonContainer*, xAOD::ShallowAuxContainer*> cpPair = 
    xAOD::shallowCopyContainer(*p_Muons);
    xAOD::MuonContainer* cpMuons = cpPair.first;
    xAOD::ShallowAuxContainer* cpAux = cpPair.second;    
    cpAux->setShallowIO( true );
    if( !xAOD::setOriginalObjectLink(*p_Muons, *cpMuons) ){
      ATH_MSG_ERROR("Failed setOriginalObjectLink");
      return StatusCode::FAILURE;
    }
    CHECK( evtStore()->record(cpMuons, m_MuonsName+sysname) );
    CHECK( evtStore()->record(cpAux, m_MuonsName+sysname+"Aux.") );

    unsigned int pIndex = 0;
    for(const auto& p : *cpMuons){
      CHECK( m_objTool->FillMuon(*p, m_muBaselinePt, m_muEta) );
      //dec_passOR(*p) = true;
      bool issig = m_objTool->IsSignalMuon(*p, m_muPt, m_mud0sig, m_muz0);
      bool iscosmic = m_objTool->IsCosmicMuon(*p, m_muCosmicz0, m_muCosmicd0);
      bool ishi = false;
      bool isbase = (bool)p->auxdata<char>("baseline");
      if( p->pt() > 4.e3 ) ishi = m_objTool->IsHighPtMuon(*p);
      if( doPrint ) ATH_MSG_DEBUG("Systematic muon pt " <<sysname <<" "
                                  <<p->pt() <<"  " <<iscosmic <<" " 
                                  <<isbase <<" " <<issig <<" " <<ishi);
      if( isbase ) (*muCutMask)[pIndex] = true;
      ++pIndex;

      const xAOD::TrackParticle* trk = 
      p->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
      if( !trk ) continue;
      size_t idx = trk->index();
      (*muSpecCutMask)[idx] = true;
    }
  }


  // Muons thinning
  //int muonsSize = p_Muons->size();
  CHECK( m_thinningSvc->filter(*p_Muons, *muCutMask, 
                               IThinningSvc::Operator::Or) );
  CHECK( m_thinningSvc->filter(*p_MuonSpecTP, *muSpecCutMask,
                               IThinningSvc::Operator::Or) );
  for(auto si : m_systInfoMUON){ const CP::SystematicSet& sys =
    si->systset; std::string sysname = sys.name(); if( sysname == "" )
    sysname = "Nominal"; const xAOD::MuonContainer* mus = 0; CHECK(
    evtStore()->retrieve(mus, m_MuonsName+sysname) );
    CHECK( m_thinningSvc->filter(*mus, *muCutMask,
                                 IThinningSvc::Operator::Or) );
  }

  ////////////
  // Electrons
  ////////////

  CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );

  // Initialize mask to false; true if any variation passes "baseline".
  // Pass m_elPtCut to FillElectron
  std::vector<bool>* elCutMask = new std::vector<bool>(p_Electrons->size(),false);
  CHECK( evtStore()->record(elCutMask, m_ElectronCutMaskName) );

  for(auto si : m_systInfoELE){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    if( doPrint ) ATH_MSG_DEBUG("Applying electron systematic " <<sysname);
    CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);

    // This largely replicates GetJets
    // setShallowIO writes output as shallow copy

    std::pair<xAOD::ElectronContainer*, xAOD::ShallowAuxContainer*> cpPair = 
    xAOD::shallowCopyContainer(*p_Electrons);
    xAOD::ElectronContainer* cpElectrons = cpPair.first;
    xAOD::ShallowAuxContainer* cpAux = cpPair.second;    
    cpAux->setShallowIO( true );
    if( !xAOD::setOriginalObjectLink(*p_Electrons, *cpElectrons) ){
      ATH_MSG_ERROR("Failed setOriginalObjectLink");
      return StatusCode::FAILURE;
    }
    CHECK( evtStore()->record(cpElectrons, m_ElectronsName+sysname) );
    CHECK( evtStore()->record(cpAux, m_ElectronsName+sysname+"Aux.") );

    unsigned int pIndex = 0;
    for(const auto& p : *cpElectrons){
      CHECK( m_objTool->FillElectron(*p, m_eleBaselinePt, m_eleEta) );
      //dec_passOR(*p) = true;
      bool issig = m_objTool->IsSignalElectron(*p, m_elePt, 
                                               m_eled0sig, m_elez0);
      if( doPrint ) ATH_MSG_DEBUG("Systematic electron pt " <<sysname <<" "
                                  <<p->pt() <<" " <<issig);
      if( p->auxdata<char>("baseline") ) (*elCutMask)[pIndex] = true;
      ++pIndex;
    }
  }

  // Electrons thinning
  CHECK( m_thinningSvc->filter(*p_Electrons, *elCutMask,
                               IThinningSvc::Operator::Or) );

  for(auto si : m_systInfoELE){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    const xAOD::ElectronContainer* els = 0;
    CHECK( evtStore()->retrieve(els, m_ElectronsName+sysname) );
    CHECK( m_thinningSvc->filter(*els, *elCutMask,
                                 IThinningSvc::Operator::Or) );
  }


  ///////////////////////////////
  // Rebuild MET with systematics
  ///////////////////////////////

  // Use nominal Electrons and Muons recalled from StoreGate
  // Ignore Photons and TauJets for MET
  // Apply JET and soft term systematics for either TST or CST

  if( doPrint ) ATH_MSG_DEBUG("Start MET rebuild");

  const xAOD::MuonContainer* p_MuonsN = 0;
  CHECK( evtStore()->retrieve(p_MuonsN, m_MuonsName+"Nominal") );

  const xAOD::ElectronContainer* p_ElectronsN = 0;
  CHECK( evtStore()->retrieve(p_ElectronsN, m_ElectronsName+"Nominal") );

  const xAOD::JetContainer* p_JetsN = 0;
  CHECK( evtStore()->retrieve(p_JetsN, m_JetsName+"Nominal") );

  if( doPrint ) ATH_MSG_DEBUG("Got Nominals for MET rebuild " <<p_MuonsN->size() <<" "
                              <<p_ElectronsN->size() <<" " <<p_JetsN->size());

  std::string METsName = m_METsName + "Trk";
  if( !m_doTST ) METsName = m_METsName + "Clus";

  // Jet systematics
  CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );

  for(auto si : m_systInfoJET){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    if( doPrint ) ATH_MSG_DEBUG("MET sysname " <<sysname);
    CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);

    const xAOD::JetContainer* p_JetsSys = 0;
    if( doPrint ) ATH_MSG_DEBUG("Using jet name " <<m_JetsName+sysname);
    CHECK( evtStore()->retrieve(p_JetsSys, m_JetsName+sysname) );
    if( doPrint ) ATH_MSG_DEBUG("Jet size " <<p_JetsSys->size());
    xAOD::MissingETContainer* met = new xAOD::MissingETContainer;
    xAOD::MissingETAuxContainer* metaux = new xAOD::MissingETAuxContainer;
    met->setStore( metaux);
    CHECK( evtStore()->record(met, METsName+sysname) );
    CHECK( evtStore()->record(metaux, METsName+sysname+"Aux.") );
    if( doPrint ) ATH_MSG_DEBUG("Recorded " <<METsName+sysname <<" "
                                <<METsName+sysname+"Aux.");
    // Only redo MET if primary vertex exists
    if( p_PrimVtx ){
      CHECK( m_objTool->GetMET(*met, p_JetsSys, p_ElectronsN, p_MuonsN, 
                               0, 0, m_doTST, m_doTST).isSuccess() );
      if( doPrint ) ATH_MSG_DEBUG("MET et " 
                    <<(*met)["Final"]->met() <<" " <<sysname);
    } else {
      ATH_MSG_DEBUG("No primary vertex, skipping GetMET");
    }
  }

  // Soft term systematics
  // No Nominal here - identical to Nominal for JET

  if( m_doTST ){
    if( doPrint ) ATH_MSG_DEBUG("Start MET TST systematics");
    for(auto si : m_systInfoTST){
      const CP::SystematicSet& sys = si->systset;
      std::string sysname = sys.name();
      if( sysname == "" ) sysname = "Nominal";
      if( doPrint ) ATH_MSG_DEBUG("MET sysname " <<sysname);
      CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);
      xAOD::MissingETContainer* met = new xAOD::MissingETContainer;
      xAOD::MissingETAuxContainer* metaux = new xAOD::MissingETAuxContainer;
      met->setStore( metaux);
      CHECK( evtStore()->record(met, METsName+sysname) );
      CHECK( evtStore()->record(metaux, METsName+sysname+"Aux.") );
      if( doPrint ) ATH_MSG_DEBUG("Recorded " <<METsName+sysname <<" "
                                  <<METsName+sysname+"Aux.");
      if( p_PrimVtx ){
        CHECK( m_objTool->GetMET(*met, p_JetsN, p_ElectronsN, p_MuonsN, 
                                 0, 0, m_doTST, m_doTST).isSuccess() );
        if( doPrint ) ATH_MSG_DEBUG("MET et " 
                      <<(*met)["Final"]->met() <<" " <<sysname);
      }
    }
  } else {
    if( doPrint ) ATH_MSG_DEBUG("Start MET CST systematics");
    for(auto si : m_systInfoCST){
      const CP::SystematicSet& sys = si->systset;
      std::string sysname = sys.name();
      if( sysname == "" ) sysname = "Nominal";
      if( doPrint ) ATH_MSG_DEBUG("MET sysname " <<sysname);
      CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);
      xAOD::MissingETContainer* met = new xAOD::MissingETContainer;
      xAOD::MissingETAuxContainer* metaux = new xAOD::MissingETAuxContainer;
      met->setStore( metaux);
      CHECK( evtStore()->record(met, METsName+sysname) );
      CHECK( evtStore()->record(metaux, METsName+sysname+"Aux.") );
      if( doPrint ) ATH_MSG_DEBUG("Recorded " <<METsName+sysname <<" "
                                  <<METsName+sysname+"Aux.");
      if( p_PrimVtx ){
        CHECK( m_objTool->GetMET(*met, p_JetsN, p_ElectronsN, p_MuonsN, 
                                 0, 0, m_doTST, m_doTST).isSuccess() );
        if( doPrint ) ATH_MSG_DEBUG("MET et " 
                      <<(*met)["Final"]->met() <<" " <<sysname);
      }
    }
  }


  //////////
  // TauJets
  //////////

  CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );

  // Selection defined by SUSYTools TauID = Loose/Medium/Tight.
  // Initialize mask to false; true if any variation passes "baseline"

  std::vector<bool>* tauCutMask = new std::vector<bool>(p_TauJets->size(),false);
  CHECK( evtStore()->record(tauCutMask, m_TauJetCutMaskName) );

  // Initialize tau truth -- needed for tau CP tools
  // Must specify TauTruthMatchingTool.WriteTruthTaus = True.

  if( !m_isData  && 
  !evtStore()->contains<xAOD::TruthParticleContainer>("TruthTaus") ){
    // If there are no taus, then we need to force the building of the container
    if (0==p_TauJets->size()) m_tauTruthTool->buildTruthTausFromTruthParticles();
    for(const auto& tau : *p_TauJets){
      const xAOD::TruthParticle* trueTau = 0;
      trueTau = m_tauTruthTool->getTruth(*tau);
      if( doPrint ){
        if( trueTau ){
          ATH_MSG_DEBUG("getTruth tau " <<tau->pt() <<" " <<tau->eta() 
                        <<" " <<tau->phi() 
                        <<" trueTau " <<trueTau->pt() <<" " <<trueTau->eta() 
                        <<" " <<trueTau->phi());
        } else {
          ATH_MSG_DEBUG("getTruth tau " <<tau->pt() <<" " <<tau->eta() 
                        <<" " <<tau->phi() << "trueTau not found");
        } 
      }
    } 
  }//end TruthTaus

  for(auto si : m_systInfoTAU){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    if( doPrint ) ATH_MSG_DEBUG("Applying tau systematic " <<sysname);
    CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);

    // This largely replicates GetTaus
    // setShallowIO writes output as shallow copy
    std::pair<xAOD::TauJetContainer*, xAOD::ShallowAuxContainer*> cpPair =
    xAOD::shallowCopyContainer(*p_TauJets);
    xAOD::TauJetContainer* cpTauJets = cpPair.first;
    xAOD::ShallowAuxContainer* cpAux = cpPair.second;
    cpAux->setShallowIO( true );
    if( !xAOD::setOriginalObjectLink(*p_TauJets, *cpTauJets) ){
      ATH_MSG_ERROR("Failed setOriginalObjectLink");
      return StatusCode::FAILURE;
    }
    CHECK( evtStore()->record(cpTauJets, m_TauJetsName+sysname) );
    CHECK( evtStore()->record(cpAux, m_TauJetsName+sysname+"Aux.") );
    if( doPrint ) ATH_MSG_DEBUG("Recorded tay shallow copy + aux for "
                                <<m_TauJetsName+sysname);

    unsigned int pIndex = 0;
    for(const auto& p : *cpTauJets){
      /*
      const xAOD::TruthParticle* trueTau = 0;
      if( !m_isData ) trueTau = m_tauTruthTool->getTruth(*p);
      if( doPrint ){
        if( trueTau ) ATH_MSG_DEBUG("Systematic truth tau " <<sysname <<" "
                      <<trueTau->pt());
        else ATH_MSG_DEBUG("Systematic truth tau NOT FOUND " <<sysname);
      }
      */
      float ptorig = (*p_TauJets)[pIndex]->pt();
      CHECK( m_objTool->FillTau(*p) );
      //bool issig = m_objTool->IsSignalTau(*p, m_tauPt, m_tauEta);
      if( doPrint ) ATH_MSG_DEBUG("Systematic tau pt " <<sysname <<" "
                                  <<p->pt() <<" " <<ptorig <<" "
                                  <<(int)p->auxdata<char>("baseline"));
      if( p->auxdata<char>("baseline") ){
        if( doPrint && sysname == "Nominal" ){
          ATH_MSG_DEBUG("Accepted Nominal tau pt " <<p->pt());
        } 
        (*tauCutMask)[pIndex] = true;
      }
      ++pIndex;
    }
  }

  // TauJets thinning
  CHECK( m_thinningSvc->filter(*p_TauJets, *tauCutMask,
                               IThinningSvc::Operator::Or) );
  for(auto si : m_systInfoTAU){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    const xAOD::TauJetContainer* taujets = 0;
    CHECK( evtStore()->retrieve(taujets, m_TauJetsName+sysname) );
    CHECK( m_thinningSvc->filter(*taujets, *tauCutMask,
                                 IThinningSvc::Operator::Or) );
  }


  //////////
  // Photons
  //////////

  CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );

  // Selection defined by SUSYTools PhotonID = Loose/Medium/Tight.
  // Initialize mask to false; true if any variation passes "baseline"
  std::vector<bool>* phoCutMask = new std::vector<bool>(p_Photons->size(),false);
  CHECK( evtStore()->record(phoCutMask, m_PhotonCutMaskName) );

  for(auto si : m_systInfoPHO){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    if( doPrint ) ATH_MSG_DEBUG("Applying photon systematic " <<sysname);
    CHECK(m_objTool->applySystematicVariation(sys) == CP::SystematicCode::Ok);

    // This largely replicates GetPhotons
    // setShallowIO writes output as shallow copy
    std::pair<xAOD::PhotonContainer*, xAOD::ShallowAuxContainer*> cpPair =
    xAOD::shallowCopyContainer(*p_Photons);
    xAOD::PhotonContainer* cpPhotons = cpPair.first;
    xAOD::ShallowAuxContainer* cpAux = cpPair.second;
    cpAux->setShallowIO( true );
    if( !xAOD::setOriginalObjectLink(*p_Photons, *cpPhotons) ){
      ATH_MSG_ERROR("Failed setOriginalObjectLink");
      return StatusCode::FAILURE;
    }
    CHECK( evtStore()->record(cpPhotons, m_PhotonsName+sysname) );
    CHECK( evtStore()->record(cpAux, m_PhotonsName+sysname+"Aux.") );
    if( doPrint ) ATH_MSG_DEBUG("Recorded photon shallow copy + aux for "
                                <<m_PhotonsName+sysname);

    unsigned int pIndex = 0;
    for(const auto& p : *cpPhotons){
      float ptorig = (*p_Photons)[pIndex]->pt();
      CHECK( m_objTool->FillPhoton(*p, m_photonBaselinePt, m_photonEta) );
      bool issig = m_objTool->IsSignalPhoton(*p, m_photonPt);
      if( doPrint ) ATH_MSG_DEBUG("Systematic photon pt " <<sysname <<" "
                                  <<p->pt() <<" " <<ptorig <<" "
                                  <<(int)p->auxdata<char>("baseline") <<" "
                                  <<issig);
      if( p->auxdata<char>("baseline") ){
        if( doPrint && sysname == "Nominal" ){
          ATH_MSG_DEBUG("Accepted Nominal photon pt " <<p->pt());
        } 
        (*phoCutMask)[pIndex] = true;
      }
      ++pIndex;
    }
  }

  // Photons thinning
  CHECK( m_thinningSvc->filter(*p_Photons, *phoCutMask,
                               IThinningSvc::Operator::Or) );
  for(auto si : m_systInfoPHO){
    const CP::SystematicSet& sys = si->systset;
    std::string sysname = sys.name();
    if( sysname == "" ) sysname = "Nominal";
    const xAOD::PhotonContainer* photons = 0;
    CHECK( evtStore()->retrieve(photons, m_PhotonsName+sysname) );
    CHECK( m_thinningSvc->filter(*photons, *phoCutMask,
                                 IThinningSvc::Operator::Or) );
  }


  ///////////////
  // InDet Tracks
  ///////////////

  // Keep InDet tracks associated with selected objects.

  const xAOD::TrackParticleContainer* p_Tracks = 0;
  CHECK( evtStore()->retrieve(p_Tracks, m_InDetTracksName) );

  // Initialize mask
  std::vector<bool>* trkCutMask = new std::vector<bool>(p_Tracks->size(),false);
  CHECK( evtStore()->record(trkCutMask, m_InDetCutMaskName) );

  // Jet ghost-associated tracks. Require pt > m_inDetPtCut and
  // Npix>=1, NSCT>=6, d0<1.5, dz0*sin(theta)<1.5
  int nTrkJet = 0;
  int nTrkTot = 0;  

  for(unsigned int j=0; j<p_Jets->size(); ++j){
    if( !(*jetCutMask)[j] ) continue;
    const xAOD::Jet* jet = (*p_Jets)[j];
    std::vector<const xAOD::IParticle*> ghosts =
    jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack);
    for(const auto& gh : ghosts){
      const xAOD::TrackParticle* ptrk = dynamic_cast<const xAOD::TrackParticle*>(gh);
      // ptrk==0 if track has been thinned
      if( !ptrk ){
        if( doPrint ) ATH_MSG_DEBUG("Could not cast ghost to track!");
        continue;
      }
      if( ptrk->pt() < m_inDetPtCut ){
        if( doPrint ) ATH_MSG_VERBOSE("JETTRACK fail pt " <<ptrk->pt());
        continue;
      }
      size_t idx = ptrk->index();
      uint8_t pix = 0;
      ptrk->summaryValue(pix, xAOD::numberOfPixelHits);
      uint8_t sct = 0;
      ptrk->summaryValue(sct, xAOD::numberOfSCTHits);
      if( pix < 1 || sct < 6 ){
        if( doPrint ) ATH_MSG_VERBOSE("JETTRACK fail hits " <<pix <<" " <<sct);
        continue;
      }
      float dz0 = ptrk->z0() + ptrk->vz() - z0PV;
      dz0 = dz0/cosh(ptrk->eta());
      if( ptrk->d0() > 1.5 || dz0 > 1.5 ){
        if( doPrint ) ATH_MSG_VERBOSE("JETTRACK fail vtx " <<ptrk->d0() <<" " <<dz0);
        continue;
      }
      (*trkCutMask)[idx] = true;
      ++nTrkJet;
    }
  }
  nTrkTot = nTrkJet;

  // Electron tracks
  // trackParticle() gives GSFTrackParticles, not InDetTrackParticles
  // Select latter with DeltaR

  const static float elMatchR = 0.05;

  for(unsigned int j=0; j<p_Electrons->size(); ++j){
    if( !(*elCutMask)[j] ) continue;
    const xAOD::Electron* el = (*p_Electrons)[j];
    for(const auto& ptrk : (*p_Tracks)){
      if( !ptrk ) continue;
      if( el->p4().DeltaR(ptrk->p4()) > elMatchR ) continue;
      size_t idx = ptrk->index();
      (*trkCutMask)[idx] = true;
      ++nTrkTot;
    }
  }

  // Muon tracks - just keep ID particle
  // No additional cuts beyond Muon reconstruction

  for(unsigned int j=0; j<p_Muons->size(); ++j){
    if( !(*muCutMask)[j] ) continue;
    const xAOD::Muon* mu = (*p_Muons)[j];
    const xAOD::TrackParticle* ptrk = 
    mu->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
    if( !ptrk ) continue;
    size_t idx = ptrk->index();
    (*trkCutMask)[idx] = true;
    ++nTrkTot;
  }

  // TauJet tracks
  // No additional cuts beyond TauJet reconstruction
  // Serban: only need trackLinks() for physics, not wide/other tracks

  for(unsigned int j=0; j<p_TauJets->size(); ++j){
    if( !(*tauCutMask)[j] ) continue;
#if ROOTCORE_RELEASE_SERIES==25
    const auto& tpLinks = (*p_TauJets)[j]->tauTrackLinks();
#else
    const auto& tpLinks = (*p_TauJets)[j]->trackLinks();
#endif
    int nTrkTau = 0;
    for(const auto& tpLink : tpLinks){
      if( !tpLink.isValid() ) continue;
      size_t idx = (*tpLink)->index();
      (*trkCutMask)[idx] = true;
      ++nTrkTot;
      ++nTrkTau;
    }
    if( doPrint ) ATH_MSG_DEBUG("nTrkTau = " <<nTrkTau);
  }

  // Call thinningSvc for tracks
  if( doPrint ) ATH_MSG_DEBUG("InDet tracks passed " <<nTrkJet <<" "
                             <<nTrkTot <<" " <<p_Tracks->size());
  CHECK( m_thinningSvc->filter(*p_Tracks, *trkCutMask, 
                               IThinningSvc::Operator::Or) );


  //////////////////////////
  // GSF Tracks and Vertices
  //////////////////////////

  // Keep GSF tracks associated with selected objects.
  // Also need GSF vertices (Photons have vertices which have tracks)

  const xAOD::TrackParticleContainer* p_gsfTracks = 0;
  CHECK( evtStore()->retrieve(p_gsfTracks, m_GSFTracksName) );
  std::vector<bool>* gsfTrackCutMask = new std::vector<bool>(p_gsfTracks->size(),false);
  CHECK( evtStore()->record(gsfTrackCutMask, m_GSFTrackCutMaskName) );

  const xAOD::VertexContainer* p_gsfVertices = 0;
  CHECK( evtStore()->retrieve(p_gsfVertices, "GSFConversionVertices") );
  std::vector<bool>* gsfVertexCutMask = new std::vector<bool>(p_gsfVertices->size(),false);
  CHECK( evtStore()->record(gsfVertexCutMask, m_GSFVertexCutMaskName) );

  for(unsigned int j=0; j<p_Electrons->size(); ++j){
    if( !(*elCutMask)[j] ) continue;
    const xAOD::Electron* el = (*p_Electrons)[j];
    size_t ntrk = el->nTrackParticles();
    for(unsigned int it=0; it<ntrk; ++it){
      const xAOD::TrackParticle* ptrk = el->trackParticle(it);
      if( !ptrk ) continue;
      if( ptrk->pt() < m_inDetPtCut ) continue;
      int idx = ptrk->index();
      (*gsfTrackCutMask)[idx] = true;
    }
  }

  for(unsigned int j=0; j<p_Photons->size(); ++j){
    if( !(*elCutMask)[j] ) continue;
    const xAOD::Photon* ph = (*p_Photons)[j];
    // GSF vertices
    for(const auto& vxLink : ph->vertexLinks()){
      if( !vxLink.isValid() ) continue;
      const xAOD::Vertex* vtx = (*vxLink);
      (*gsfVertexCutMask)[vtx->index()] = true;
      // GSF tracks from vertices
      for(const auto& trkLink : vtx->trackParticleLinks()){
        if( !trkLink.isValid() ) continue;
        const xAOD::TrackParticle* ptrk = (*trkLink);
        (*gsfTrackCutMask)[ptrk->index()] = true;
      }
    }
  }

  // Do thinning
  CHECK( m_thinningSvc->filter(*p_gsfTracks, *gsfTrackCutMask, 
                               IThinningSvc::Operator::Or) );
  CHECK( m_thinningSvc->filter(*p_gsfVertices, *gsfVertexCutMask, 
                               IThinningSvc::Operator::Or) );


  /////////////
  // Truth Jets
  /////////////

  // Only for Monte Carlo
  if( m_TruthJetsName != "" && m_isData==0 ){
    const xAOD::JetContainer* p_TruthJets = 0;
    CHECK( evtStore()->retrieve(p_TruthJets, m_TruthJetsName) );
    if( doPrint ) ATH_MSG_DEBUG("Got p_TruthJets size =  " <<p_TruthJets->size());

    std::vector<bool>* truthJetCutMask = new std::vector<bool>(p_TruthJets->size(),false);
    CHECK( evtStore()->record(truthJetCutMask, m_TruthJetCutMaskName) );

    unsigned int pIndex = 0;
    for(const auto& p : *p_TruthJets){
      if( p->pt() > m_jetPtCut ) (*truthJetCutMask)[pIndex] = true;
      ++pIndex;
    }

    CHECK( m_thinningSvc->filter(*p_TruthJets, *truthJetCutMask, 
                                 IThinningSvc::Operator::Or) );
  }//end m_TruthJetsName


  //////////
  // Trigger
  //////////

  // DerivationFramework trigger thinning is complicated. Just support
  // TrigDecisionTool and flag matched Electrons and Muons.
  CHECK( m_objTool->resetSystematics() == CP::SystematicCode::Ok );
  if( doPrint ) ATH_MSG_DEBUG("Start trigger");

  for( const auto& key : m_ElectronTrigs ){
    if( doPrint ) ATH_MSG_DEBUG("e trigger " <<key);
    SG::AuxElement::Decorator<char>dec_match("match"+key);
    for(const auto& p : (*p_Electrons) ){
      dec_match(*p) = m_objTool->IsTrigMatched(p, key);
      if( doPrint ) ATH_MSG_DEBUG("e match " <<p->pt() <<" " <<p->eta() <<" "
                                             <<(int)dec_match(*p));
    }
  }

  for( const auto& key : m_MuonTrigs ){
    if( doPrint ) ATH_MSG_DEBUG("mu trigger " <<key);
    SG::AuxElement::Decorator<char>dec_match("match"+key);
    for(const auto& p : (*p_Muons) ){
      dec_match(*p) = m_objTool->IsTrigMatched(p, key);
      if( doPrint ) ATH_MSG_DEBUG("mu match " <<p->pt() <<" " <<p->eta() <<" "
                                              <<(int)dec_match(*p));
    }
  }

  for( const auto& key : m_PhotonTrigs ){
    if( doPrint ) ATH_MSG_DEBUG("e trigger " <<key);
    SG::AuxElement::Decorator<char>dec_match("match"+key);
    for(const auto& p : (*p_Photons) ){
      dec_match(*p) = m_objTool->IsTrigMatched(p, key);
      if( doPrint ) ATH_MSG_DEBUG("g match " <<p->pt() <<" " <<p->eta() <<" "
                                             <<(int)dec_match(*p));
    }
  }

  ///////
  // Done
  ///////

  // Cleanup handled by StoreGate
  if( doPrint ) ATH_MSG_DEBUG("End ApplySUSYTools for event " <<m_evtCount);
  return StatusCode::SUCCESS;

} //end execute()

} //end ST namespace

