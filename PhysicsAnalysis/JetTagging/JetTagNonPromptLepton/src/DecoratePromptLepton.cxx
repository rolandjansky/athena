/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// ROOT
#include "TMVA/Config.h"

// Local
#include "DecoratePromptLepton.h"


//======================================================================================================
Prompt::DecoratePromptLepton::DecoratePromptLepton(const std::string& name,
						   ISvcLocator* pSvcLocator):  
  AthAlgorithm(name, pSvcLocator),
  m_TMVAReader            (0),
  m_TMVAReaderOneTrack    (0),
  m_TMVAReaderThreeTrack  (0),
  m_accessCalIsolation20  (0),
  m_accessCalIsolation30  (0),
  m_accessTrackIsolation30(0),
  m_accessSV1Vertices     (0),
  m_decoratorBDT          (0),
  m_decoratorDR           (0)
{
  declareProperty("LeptonContainerName",         m_leptonContainerName);
  declareProperty("TrackJetContainerName",       m_trackJetContainerName);
  declareProperty("ConfigFileVersion",           m_configFileVersion);
  declareProperty("ConfigFileVersionOneTrack",   m_configFileVersionOneTrack);
  declareProperty("ConfigFileVersionThreeTrack", m_configFileVersionThreeTrack);
  declareProperty("ConfigPathOverride",          m_configPathOverride);
  declareProperty("AuxVarPrefix",                m_auxVarPrefix);
  declareProperty("BDTName",                     m_BDTName);
  declareProperty("MethodTitleMVA",              m_methodTitleMVA = "BDT");
  declareProperty("MethodTitleMVAOneTrack",      m_methodTitleMVAOneTrack   = "BDT");
  declareProperty("MethodTitleMVAThreeTrack",    m_methodTitleMVAThreeTrack = "BDT");

  declareProperty("StringIntVars",               m_stringIntVars);
  declareProperty("StringFloatVars",             m_stringFloatVars);
  declareProperty("PrintAuxVars",                m_printAuxVars = false); 
  declareProperty("PrintTime",                   m_printTime    = false);
}


//=============================================================================
StatusCode Prompt::DecoratePromptLepton::initialize()
{   
  if(m_printTime) {
    //
    // Reset timers
    //
    m_timerAll .Reset();
    m_timerExec.Reset();
    m_timerMuon.Reset();
    m_timerElec.Reset();
    m_timerTau .Reset();
    m_timerJet .Reset();

    //
    // Start full timer
    //
    m_timerAll.Start();
  }

  //
  // Read vector<string> vars into vector<Var> and append
  //
  m_intVars   = Prompt::Def::ReadVectorVars(m_stringIntVars);
  m_floatVars = Prompt::Def::ReadVectorVars(m_stringFloatVars);

  m_allVars.insert(m_allVars.end(), m_intVars  .begin(), m_intVars  .end());
  m_allVars.insert(m_allVars.end(), m_floatVars.begin(), m_floatVars.end());

  //
  // Fill decorator maps
  //
  initializeDecorators();

  //
  // Initialize const accessors
  //
  initializeConstAccessors();

  //
  // Initialize TMVA Reader - different function needed for taus
  //
  bool success = false;

  if(m_leptonContainerName == "TauJets") {
    success = initializeTMVAReaderTau();
  }
  else {
    success = initializeTMVAReader();
  }

  if(success) return StatusCode::SUCCESS;
  else        return StatusCode::FAILURE;
}


//=============================================================================
StatusCode Prompt::DecoratePromptLepton::finalize()
{
  if(m_printTime) {
    //
    // Print full time stopwatch
    //
    m_timerAll.Stop();

    ATH_MSG_INFO("Real time: " << m_timerAll.RealTime() << "\t CPU time: " << m_timerAll.CpuTime());

    ATH_MSG_INFO("Execute time: " << PrintResetStopWatch(m_timerExec));
    ATH_MSG_INFO("Muon    time: " << PrintResetStopWatch(m_timerMuon));
    ATH_MSG_INFO("Elec    time: " << PrintResetStopWatch(m_timerElec));
    ATH_MSG_INFO("Tau     time: " << PrintResetStopWatch(m_timerTau ));
    ATH_MSG_INFO("Jet     time: " << PrintResetStopWatch(m_timerJet ));
  }

  //
  // Delete pointers
  //
  if(m_leptonContainerName == "TauJets") {
    delete m_TMVAReaderOneTrack;
    delete m_TMVAReaderThreeTrack;
  }
  else {
    delete m_TMVAReader;
  }
  
  delete m_accessCalIsolation20;
  delete m_accessCalIsolation30;
  delete m_accessTrackIsolation30;
  delete m_accessSV1Vertices;
  delete m_decoratorBDT;
  delete m_decoratorDR;

  for(Float_t *var: m_varTMVA) {
    delete var;
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
StatusCode Prompt::DecoratePromptLepton::execute()
{  
  //
  // Start execute timer
  //
  TimerScopeHelper timer(m_timerExec);

  //
  // Retrieve containers from evtStore
  //
  const xAOD::JetContainer* trackJetContainer = 0;
	
  if(evtStore() -> retrieve(trackJetContainer, m_trackJetContainerName).isFailure() || !trackJetContainer) {
    ATH_MSG_ERROR("Failed to get: " << m_trackJetContainerName);
    return StatusCode::FAILURE;
  }
  
  const xAOD::IParticleContainer* leptonContainer = 0;
	
  if(evtStore() -> retrieve(leptonContainer, m_leptonContainerName).isFailure() || !leptonContainer) {
    ATH_MSG_ERROR("Failed to get: " << m_leptonContainerName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("=======================================" 
		<< "\n\t\t\t  Size of lepton container:    " << leptonContainer  ->size() 
		<< "\n\t\t\t  Size of track jet container: " << trackJetContainer->size()
		<< "\n-----------------------------------------------------------------");

  if(m_leptonContainerName == "Electrons") {
    //
    // Dynamic cast IParticle container to Electron container and decorate
    //
    m_electrons = dynamic_cast< const xAOD::ElectronContainer* >(leptonContainer);

    for(const xAOD::Electron *elec: *m_electrons) {
      decorateElectron(elec, trackJetContainer);
    }  
  }

  else if(m_leptonContainerName == "Muons") {     
    //
    // Dynamic cast IParticle container to muon container and decorate
    //
    m_muons = dynamic_cast< const xAOD::MuonContainer* >(leptonContainer);

    for(const xAOD::Muon *muon: *m_muons) {
      decorateMuon(muon, trackJetContainer);
    }  
  }

  else if(m_leptonContainerName == "TauJets") {     
    //
    // Dynamic cast IParticle container to tau container and decorate
    //
    m_taus = dynamic_cast< const xAOD::TauJetContainer* >(leptonContainer);

    for(const xAOD::TauJet *tau: *m_taus) {
      decorateTau(tau, trackJetContainer);
    }  
  }

  else {
    ATH_MSG_ERROR("Must specify Electrons or Muons");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
bool Prompt::DecoratePromptLepton::initializeTMVAReader()
{   
  //
  // Make new instance of TMVA Reader and add variables
  //
  m_TMVAReader = new TMVA::Reader();

  for(Prompt::Def::Var &var: m_allVars) {
    Float_t *new_var = new Float_t(0.0);
    m_TMVAReader->AddVariable(Prompt::Def::AsStr(var), new_var);   
    m_varTMVA.push_back(new_var);
  }
  
  //
  // Get path to xml training file
  //
  std::string fullPathToFile = PathResolverFindCalibFile("JetTagNonPromptLepton/" 
							 + m_configFileVersion 
							 + "/TMVAClassification_" + m_methodTitleMVA + ".weights.xml");
 
  if(!m_configPathOverride.empty()) {
    ATH_MSG_INFO("Override path resolver result");
    fullPathToFile = m_configPathOverride;
  }

  ATH_MSG_INFO("TMVA configuration file: " + fullPathToFile);
  ATH_MSG_INFO("TMVA method name: " + m_methodTitleMVA);

  //
  // Book an instance of BDT reader if path is available
  //
  if(fullPathToFile == "") {
    ATH_MSG_ERROR("Could not find path to xml training file");
    return false;
  }
  else {
    m_TMVAReader->BookMVA(m_methodTitleMVA, fullPathToFile);
  }

  return true;
}

//=============================================================================
bool Prompt::DecoratePromptLepton::initializeTMVAReaderTau()
{   
  //
  // Make new instances of TMVA Readers and add variables
  //
  m_TMVAReaderOneTrack   = new TMVA::Reader();
  m_TMVAReaderThreeTrack = new TMVA::Reader();

  for(Prompt::Def::Var &var: m_allVars) {
    Float_t *new_var = new Float_t(0.0);

    m_TMVAReaderOneTrack  ->AddVariable(Prompt::Def::AsStr(var), new_var);
    m_TMVAReaderThreeTrack->AddVariable(Prompt::Def::AsStr(var), new_var);

    m_varTMVA.push_back(new_var);
  }
  
  //
  // Get path to xml training files
  //
  std::string fullPathToFileOneTrack = PathResolverFindCalibFile("JetTagNonPromptLepton/" 
								 + m_configFileVersionOneTrack 
								 + "/TMVAClassification_" + m_methodTitleMVAOneTrack + ".weights.xml");
  std::string fullPathToFileThreeTrack = PathResolverFindCalibFile("JetTagNonPromptLepton/" 
								   + m_configFileVersionThreeTrack 
								   + "/TMVAClassification_" + m_methodTitleMVAThreeTrack + ".weights.xml");

  ATH_MSG_INFO("TMVA configuration file (one track): " + fullPathToFileOneTrack);
  ATH_MSG_INFO("TMVA method name (one track):        " + m_methodTitleMVAOneTrack);

  ATH_MSG_INFO("TMVA configuration file (three tracks): " + fullPathToFileThreeTrack);
  ATH_MSG_INFO("TMVA method name (three tracks):        " + m_methodTitleMVAThreeTrack);

  //
  // Book an instance of BDT reader if path is available
  //
  if(fullPathToFileOneTrack == "" || fullPathToFileThreeTrack == "") {
    ATH_MSG_ERROR("Could not find path to xml training file");
    return false;
  }
  else {
    m_TMVAReaderOneTrack  ->BookMVA(m_methodTitleMVAOneTrack,   fullPathToFileOneTrack);
    m_TMVAReaderThreeTrack->BookMVA(m_methodTitleMVAThreeTrack, fullPathToFileThreeTrack);
  }

  return true;
}

//=============================================================================
void Prompt::DecoratePromptLepton::initializeDecorators()
{   
  //
  // Fill short variable map
  //
  for(Prompt::Def::Var &var: m_intVars) {
    SG::AuxElement::Decorator<short> shortDecorator(m_auxVarPrefix+Prompt::Def::AsStr(var));
    if(!m_shortMap.insert(shortDecoratorMap::value_type(var, shortDecorator)).second) {
      ATH_MSG_WARNING("Instantiation of Decorator class failed for short decorator map");
    }
  }

  //
  // Fill float variable map
  //
  for(Prompt::Def::Var &var: m_floatVars) {
    SG::AuxElement::Decorator<float> floatDecorator(m_auxVarPrefix+Prompt::Def::AsStr(var));
    if(!m_floatMap.insert(floatDecoratorMap::value_type(var, floatDecorator)).second) {
      ATH_MSG_WARNING("Instantiation of Decorator class failed for float decorator map");
    }
  }

  //
  // Instantiate decorators
  //
  m_decoratorBDT = new SG::AuxElement::Decorator<float>(m_BDTName);
  m_decoratorDR  = new SG::AuxElement::Decorator<float>(m_auxVarPrefix + "DRlj");
}

//=============================================================================
void Prompt::DecoratePromptLepton::initializeConstAccessors()
{   
  //
  // Instantiate accessors
  //
  m_accessCalIsolation20   = new AccessFloat ("topoetcone20");
  m_accessCalIsolation30   = new AccessFloat ("topoetcone30");
  m_accessTrackIsolation30 = new AccessFloat ("ptvarcone30");
  m_accessSV1Vertices      = new AccessVertex("SV1_vertices"); 
}

//=============================================================================
void Prompt::DecoratePromptLepton::decorateElectron(const xAOD::Electron* electron, 
						    const xAOD::JetContainer* trackJets)
{ 
  //
  // Find nearest track jet to electron
  //
  TimerScopeHelper timer(m_timerElec);

  std::pair<double, const xAOD::Jet*> match = FindNearestTrackJet(electron, trackJets);

  //
  // Get jet variables and return ntrack in track jet
  //
  Prompt::VarHolder vars;
  const int ntrack = getJetVariables(match.second, vars);

  if(match.second && ntrack >= 0) {    
    //
    // Get electron variables provided track jet has b-tagging pointer
    //
    getLeptonVariables(electron, vars);

    // Get mutual variables, passing track as argument 
    // (different for electrons and muons and not available in IParticle form)
    const xAOD::TrackParticle* track = electron->trackParticle();
    getMutualVariables(electron, match.second, track, vars, match.first);

    //
    // Add variables to TMVA Reader
    //
    bool goodJet = true;

    if(match.first < 0.4 && ntrack > 0) {
      addVarsToTMVA(vars);
    }
    else {
      goodJet = false;
    }

    //
    // Decorate electron with input vars and BDT weight
    //
    decorateAuxLepton(electron, vars, goodJet);

    (*m_decoratorDR)(*electron) = float(match.first);
  }
  else {
    ATH_MSG_DEBUG("No track jet found near to electron");
    //
    // Decorate electron with default values
    //
    fillVarDefault(vars);
    decorateAuxLepton(electron, vars, false);

    (*m_decoratorDR)(*electron) = float(-1.0);
  }

  ATH_MSG_DEBUG("Electron BDT score: " << electron->auxdataConst<float>(m_BDTName)
		<< "\n-----------------------------------------------------------------");

  if(m_printAuxVars) {
    printAuxVars(electron);
  }
}


//=============================================================================
void Prompt::DecoratePromptLepton::decorateMuon(const xAOD::Muon* muon, 
						const xAOD::JetContainer* trackJets)
{ 
  //
  // Find nearest track jet to muon
  //
  TimerScopeHelper timer(m_timerMuon);

  std::pair<double, const xAOD::Jet *> match = FindNearestTrackJet(muon, trackJets);

  //
  // Get jet variables and return ntrack in track jet
  //
  Prompt::VarHolder vars;
  const int ntrack = getJetVariables(match.second, vars);

  if(match.second && ntrack >= 0) {
    //
    // Get muon variables provided track jet has b-tagging pointer
    //
    getLeptonVariables(muon, vars);

    // Get mutual variables, passing track as argument 
    // (different for electrons and muons and not available in IParticle form)
    const xAOD::TrackParticle* track = muon->primaryTrackParticle();
    getMutualVariables(muon, match.second, track, vars, match.first);

    //
    // Add variables to TMVA Reader
    //
    bool goodJet = true;

    if(match.first < 0.4 && ntrack > 0) {
      addVarsToTMVA(vars);
    }
    else {
      goodJet = false;
    }

    //
    // Decorate muon with input vars and BDT weight
    //
    decorateAuxLepton(muon, vars, goodJet);

    (*m_decoratorDR)(*muon) = float(match.first);
  }
  else {
    ATH_MSG_DEBUG("No track jet found near to muon");
    //
    // Decorate muon with default values
    //
    fillVarDefault(vars);
    decorateAuxLepton(muon, vars, false);

    (*m_decoratorDR)(*muon) = float(-1.0);
  }

  ATH_MSG_DEBUG("Muon BDT score: " << muon->auxdataConst<float>(m_BDTName) 
		<< "\n-----------------------------------------------------------------");

  if(m_printAuxVars) {
    printAuxVars(muon);
  }
}


//=============================================================================
void Prompt::DecoratePromptLepton::decorateTau(const xAOD::TauJet* tau, 
					       const xAOD::JetContainer* trackJets)
{ 
  //
  // Find nearest track jet to tau
  //
  TimerScopeHelper timer(m_timerTau);

  std::pair<double, const xAOD::Jet *> match = FindNearestTrackJet(tau, trackJets);

  // Get ntrack in tau to decide which reader to evaluate BDT on
  const int ntracktau = tau->nTracks();

  //
  // Get jet variables and return ntrack in track jet
  //
  Prompt::VarHolder vars;
  const int ntrack = getJetVariables(match.second, vars);

  if(match.second && ntrack >= 0 && ntracktau >= 0) {

    // Get mutual tau-track jet variables
    getMutualTauVariables(tau, match.second, vars, match.first);

    //
    // Add variables to TMVA Reader
    //
    bool goodJet = true;

    if(match.first < 0.4 && ntrack > 0) {
      addVarsToTMVA(vars);
    }
    else {
      goodJet = false;
    }

    //
    // Decorate tau with input vars and BDT weight
    //
    decorateAuxTau(tau, vars, goodJet, ntracktau);

    (*m_decoratorDR)(*tau) = float(match.first);
  }
  else {
    ATH_MSG_DEBUG("No track jet found near to tau");
    //
    // Decorate tau with default values
    //
    fillVarDefault(vars);
    decorateAuxTau(tau, vars, false, ntracktau);

    (*m_decoratorDR)(*tau) = float(-1.0);
  }

  ATH_MSG_DEBUG("Tau BDT score: " << tau->auxdataConst<float>(m_BDTName) 
		<< "\n-----------------------------------------------------------------");

  if(m_printAuxVars) {
    printAuxVars(tau);
  }
}


//=============================================================================
int Prompt::DecoratePromptLepton::getJetVariables(const xAOD::Jet* jet, Prompt::VarHolder &vars)
{
  //
  // Check jet has a btagging pointer
  //
  TimerScopeHelper timer(m_timerJet);

 if(!jet) {
    ATH_MSG_DEBUG("Track jet is null pointer");
    return -1;
  }

  const xAOD::BTagging *btag = jet->btagging();
  if(!btag) {
    ATH_MSG_DEBUG("Track jet has no b-tagging pointer");
    return -2;
  }

  //
  // NTrack, SV1 and JetFitter
  //
  const int ntrack = jet->getConstituents().size();

  int sv1_ntkv = 0;
  float    SV1 = -99.0;

  if(m_accessSV1Vertices->isAvailable(*btag)) {
    const std::vector<ElementLink<xAOD::VertexContainer > > SV1vertices = (*m_accessSV1Vertices)(*btag);
    if(SV1vertices.size() != 0) {     
      btag->taggerInfo(sv1_ntkv, xAOD::SV1_NGTinSvx);
    }
  }
  else {
    ATH_MSG_WARNING("SV1 vertex container not found in auxiliary store");
  }  

  // SV1 LLR
  SV1 = btag->SV1_loglikelihoodratio();

  int jf_ntrkv  =  0;
  int tmpNvtx   = -1;
  int tmpNvtx1t = -1;
  float    JetF = -99.0;

  btag->taggerInfo(tmpNvtx,   xAOD::JetFitter_nVTX);
  btag->taggerInfo(tmpNvtx1t, xAOD::JetFitter_nSingleTracks);

  if (tmpNvtx > 0 || tmpNvtx1t > 0) {
    btag->taggerInfo(jf_ntrkv, xAOD::JetFitter_nTracksAtVtx);
  }

  // JetFitter LLR
  JetF = btag->JetFitter_loglikelihoodratio();

  //
  // Add new variable which is the addition of sv1 + jf
  //
  int sv1_jf_ntrkv = sv1_ntkv + jf_ntrkv;

  //
  // IP2D and IP3D
  //
  float ip2 = -99., ip3 = -99., ip2_cu = -99., ip3_cu = -99.;

  float ip2_pc = btag->IP2D_pc();
  float ip2_pu = btag->IP2D_pu();
  float ip3_pc = btag->IP3D_pc();
  float ip3_pu = btag->IP3D_pu();

  ip2_cu = ip2_pc>0 and ip2_pu ? log(ip2_pc/ip2_pu) : -20;
  ip3_cu = ip3_pc>0 and ip3_pu ? log(ip3_pc/ip3_pu) : -20;

  // IP LLR
  ip2 = btag->IP2D_loglikelihoodratio();
  ip3 = btag->IP3D_loglikelihoodratio();

  //
  // r21 LLR variables
  //
  std::vector<VarPair> LLRVars = {std::make_pair(Prompt::Def::DL1mu, -100.0),
				  std::make_pair(Prompt::Def::rnnip, -100.0)};

  for(VarPair &var: LLRVars) { 
    if(!btag->loglikelihoodratio(Prompt::Def::AsStr(var.first), var.second)) {	
      ATH_MSG_WARNING("Missing " << Prompt::Def::AsStr(var.first));
    }

    //
    // Add LLR vars to VarHolder
    //
    vars.AddVar(var.first, var.second);
  }

  //
  // r21 MV2 vars
  //
  std::vector<VarPair> MV2Vars = {std::make_pair(Prompt::Def::MV2c10rnn, -100.0)};

  for(VarPair &var: MV2Vars) {
    if(!btag->MVx_discriminant(Prompt::Def::AsStr(var.first), var.second)) {	
      ATH_MSG_WARNING("Missing " << Prompt::Def::AsStr(var.first));
    }

    //
    // Add MV2 vars to VarHolder
    //
    vars.AddVar(var.first, var.second);
  }
  
  //
  // Add vars to VarHolder
  //
  vars.AddVar(Prompt::Def::TrackJetNTrack, ntrack);
  vars.AddVar(Prompt::Def::sv1_ntkv,       sv1_ntkv);
  vars.AddVar(Prompt::Def::jf_ntrkv,       jf_ntrkv);
  vars.AddVar(Prompt::Def::sv1_jf_ntrkv,   sv1_jf_ntrkv);
  vars.AddVar(Prompt::Def::ip2,            ip2);
  vars.AddVar(Prompt::Def::ip2_cu,         ip2_cu);
  vars.AddVar(Prompt::Def::ip3,            ip3);
  vars.AddVar(Prompt::Def::ip3_cu,         ip3_cu);
  vars.AddVar(Prompt::Def::SV1,            SV1);
  vars.AddVar(Prompt::Def::JetF,           JetF);

  if(ntrack == 1) {
    ATH_MSG_DEBUG("Nearest track jet only contains one track");
  }

  return ntrack;
}

//=============================================================================
void Prompt::DecoratePromptLepton::getLeptonVariables(const xAOD::IParticle* particle, 
						      Prompt::VarHolder &vars)
{
  //
  // Get lepton variables - isolation
  //
  float topoetcone20rel = -99.0, topoetcone30rel = -99.0, ptvarcone30rel = -99.0;

  topoetcone20rel = accessIsolation(&(*m_accessCalIsolation20),   &(*particle));
  topoetcone30rel = accessIsolation(&(*m_accessCalIsolation30),   &(*particle));
  ptvarcone30rel  = accessIsolation(&(*m_accessTrackIsolation30), &(*particle));

  vars.AddVar(Prompt::Def::EtTopoCone20Rel, topoetcone20rel);  
  vars.AddVar(Prompt::Def::EtTopoCone30Rel, topoetcone30rel);  
  vars.AddVar(Prompt::Def::TopoEtCone30Rel, topoetcone30rel);  
  vars.AddVar(Prompt::Def::PtVarCone30Rel,  ptvarcone30rel);  
}

//=============================================================================
void Prompt::DecoratePromptLepton::getMutualVariables(const xAOD::IParticle* particle,
						      const xAOD::Jet* jet, 
						      const xAOD::TrackParticle* track,
						      Prompt::VarHolder &vars,
						      float DRlj)
{
  //
  // Add lepton - jet variables to VarHolder
  //
  float LepJetPtFrac = -99.;
  float PtFrac       = -99.;
  float PtRel        = -99.;
 
  if(particle->pt() > 0.0 && jet->pt() > 0.0) {
    LepJetPtFrac = particle->pt() / jet->pt();

    if(track) {
      PtFrac = track->pt() / jet->pt();
    }

    float angle = particle->p4().Vect().Angle(jet->p4().Vect());
    PtRel = particle->pt() * std::sin(angle);
  }

  //
  // Add vars to VarHolder
  //
  vars.AddVar(Prompt::Def::LepJetPtFrac, LepJetPtFrac);
  vars.AddVar(Prompt::Def::PtFrac,       PtFrac);
  vars.AddVar(Prompt::Def::PtRel,        PtRel);
  vars.AddVar(Prompt::Def::DRlj,         DRlj);
}

//=============================================================================
void Prompt::DecoratePromptLepton::getMutualTauVariables(const xAOD::TauJet* tau,
							 const xAOD::Jet* jet, 
							 Prompt::VarHolder &vars,
							 float DRlj)
{
  //
  // Add tau - jet variables to VarHolder
  //
  float LepJetPtFrac = -99.;
 
  if(tau->pt() > 0.0 && jet->pt() > 0.0) {
    LepJetPtFrac = tau->pt() / jet->pt();
  }

  //
  // Add vars to VarHolder
  //
  vars.AddVar(Prompt::Def::LepJetPtFrac, LepJetPtFrac);
  vars.AddVar(Prompt::Def::DRlj,         DRlj);
}

//=============================================================================
float Prompt::DecoratePromptLepton::accessIsolation(AccessFloat* isoAccessor, 
						    const xAOD::IParticle* particle)
{
  float isolation = -99., isolationrel = -99.;
  
  if(isoAccessor->isAvailable(*particle)) {
    isolation = (*isoAccessor)(*particle);
  }
  else {
    ATH_MSG_WARNING("Lepton isolation not found in auxiliary store");
  }

  if(particle->pt() > 0.0) {
    isolationrel = isolation / particle->pt();
  }

  return isolationrel;
}

//=============================================================================
void Prompt::DecoratePromptLepton::addVarsToTMVA(Prompt::VarHolder &vars)
{
  //
  // Add variables to TMVA reader
  //
  for(unsigned i = 0; i < m_allVars.size(); ++i) {  
    if(!vars.GetVar(m_allVars.at(i), *m_varTMVA[i])) {
      ATH_MSG_ERROR("Missing input variable: " << Prompt::Def::AsStr(m_allVars.at(i)));   
    }
  }
}

//=============================================================================
void Prompt::DecoratePromptLepton::fillVarDefault(Prompt::VarHolder &vars)
{
  //
  // Add default values to VarHolder
  //
  for(Prompt::Def::Var &var: m_allVars) {  
    vars.AddVar(var, -99.0);
  }
}

//=============================================================================
void Prompt::DecoratePromptLepton::decorateAuxLepton(const xAOD::IParticle* particle, 
						     Prompt::VarHolder &vars, 
						     bool goodJet)
{
  //
  // Decorate lepton with input short variables
  //
  for(shortDecoratorMap::value_type &dec: m_shortMap) {
    double val = 0.0;

    if(vars.GetVar(dec.first, val)) {
      dec.second(*particle) = static_cast<short>(val);
    }    
    else {
      ATH_MSG_WARNING("Variable " << Prompt::Def::AsStr(dec.first) << " not decorated to lepton");
    }

    ATH_MSG_DEBUG("Variable: " << Prompt::Def::AsStr(dec.first) << "\tValue: " 
		  << particle->auxdataConst<short>(m_auxVarPrefix+Prompt::Def::AsStr(dec.first)));
  }  

  //
  // Decorate lepton with input float variables
  //
  for(floatDecoratorMap::value_type &dec: m_floatMap) {
    double val = 0.0;

    if(vars.GetVar(dec.first, val)) {
      dec.second(*particle) = val;
    }
    else {
      ATH_MSG_WARNING("Variable " << Prompt::Def::AsStr(dec.first) << " not decorated to lepton");
    }

    ATH_MSG_DEBUG("Variable: " << Prompt::Def::AsStr(dec.first) << "\tValue: " 
		  << particle->auxdataConst<float>(m_auxVarPrefix+Prompt::Def::AsStr(dec.first)));
  }

  //
  // Decorate lepton with classifier response, if goodJet
  //
  if(goodJet) {
    (*m_decoratorBDT)(*particle) = static_cast<float>(m_TMVAReader->EvaluateMVA(m_methodTitleMVA));
  }
  else {
    ATH_MSG_DEBUG("No nearby track jet, DR > 0.4");
    (*m_decoratorBDT)(*particle) = float(-1.1);
  }
}

//=============================================================================
void Prompt::DecoratePromptLepton::decorateAuxTau(const xAOD::TauJet* tau, 
						  Prompt::VarHolder &vars, 
						  bool goodJet,
						  const int ntracktau)
{
  ATH_MSG_DEBUG("Tau (pT [GeV], NTracks): " << tau->pt()/1000 << ", " << ntracktau);

  //
  // Decorate lepton with input short variables
  // isAvailable check not needed since only one tau alg ran
  //
  for(shortDecoratorMap::value_type &dec: m_shortMap) {
    double val = 0.0;

    if(vars.GetVar(dec.first, val)) {
      dec.second(*tau) = static_cast<short>(val);
    }    
    else {
      ATH_MSG_WARNING("Variable " << Prompt::Def::AsStr(dec.first) << " not decorated to tau");
    }

    ATH_MSG_DEBUG("Variable: " << Prompt::Def::AsStr(dec.first) << "\tValue: " 
		  << tau->auxdataConst<short>(m_auxVarPrefix+Prompt::Def::AsStr(dec.first)));
  }  

  //
  // Decorate lepton with input float variables
  //
  for(floatDecoratorMap::value_type &dec: m_floatMap) {
    double val = 0.0;

    if(vars.GetVar(dec.first, val)) {
      dec.second(*tau) = val;
    }
    else {
      ATH_MSG_WARNING("Variable " << Prompt::Def::AsStr(dec.first) << " not decorated to tau");
    }

    ATH_MSG_DEBUG("Variable: " << Prompt::Def::AsStr(dec.first) << "\tValue: " 
		  << tau->auxdataConst<float>(m_auxVarPrefix+Prompt::Def::AsStr(dec.first)));
  }

  //
  // Decorate lepton with classifier response, if goodJet and ntracktau = 1 or 3
  // Different trainings/readers for different ntracktau
  //
  if(goodJet && (ntracktau == 1 || ntracktau == 3)) {
    if(ntracktau == 1) {
      (*m_decoratorBDT)(*tau) = static_cast<float>(m_TMVAReaderOneTrack->EvaluateMVA(m_methodTitleMVAOneTrack));
    }
    else if(ntracktau == 3) {
      (*m_decoratorBDT)(*tau) = static_cast<float>(m_TMVAReaderThreeTrack->EvaluateMVA(m_methodTitleMVAThreeTrack));
    }
  }
  else {
    //
    // Decorate with default values
    //
    if(!(ntracktau == 1 || ntracktau == 3)) {
      ATH_MSG_DEBUG("Tau does not have 1 or 3 tracks");
      (*m_decoratorBDT)(*tau) = float(-1.2);
    }
    else {
      ATH_MSG_DEBUG("No nearby track jet, DR > 0.4");
      (*m_decoratorBDT)(*tau) = float(-1.1);
    }
  }
}

//=============================================================================
template<class T> std::pair<double, const xAOD::Jet*>
Prompt::DecoratePromptLepton::FindNearestTrackJet(const T &part, const xAOD::JetContainer* jets)
{
  //
  // Find nearest track jet and a return a pair of dR and xAOD::Jet*
  //
  xAOD::Jet* ptr = 0;
  
  std::pair<double, const xAOD::Jet*> match(0.0, ptr);
	
  for(const xAOD::Jet* jet: *jets) {
    const double dr = part->p4().DeltaR(jet->p4());
    
    if(match.second) {
      if(dr < match.first) {
	match.first  = dr;
	match.second = jet;
      }
    }
    else {
      match.first  = dr;
      match.second = jet;     
    }
  }
	   
  return match;
}

//=============================================================================
void Prompt::DecoratePromptLepton::printAuxVars(const xAOD::IParticle* particle) const
{
  //ptr->auxdataConst<float>(itype);
  
  for( SG::auxid_t auxid : particle->getAuxIDs() ) {
    static SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
    
    std::stringstream str;

    str << "  AUX var name:  " << reg.getName( auxid ) 
	<< "  typename="       << reg.getTypeName( auxid )
	<< "  type="           << reg.getType( auxid ) 
	<< "  value=?";

    ATH_MSG_INFO(str.str());
  }
}
