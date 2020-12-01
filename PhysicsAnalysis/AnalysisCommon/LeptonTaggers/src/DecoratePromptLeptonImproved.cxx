/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

// ROOT
#include "TMVA/Config.h"
#include "TH1.h"

// Local
#include "DecoratePromptLeptonImproved.h"

//======================================================================================================
Prompt::DecoratePromptLeptonImproved::DecoratePromptLeptonImproved(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_BDTVarKey(Def::NONE)
{
  declareProperty("LeptonContainerName",         m_leptonsName,                     "Container's name of the lepton that you want to decorate");
  declareProperty("TrackJetContainerName",       m_trackJetsName,                   "Track Jet container name");
  declareProperty("PrimaryVertexContainerName",  m_primaryVertexName,               "Primary vertex container name");
  declareProperty("ClusterContainerName",        m_clusterContainerName,            "Container name of the Clusters which used to calculate the input variables for the PromptLeptonImproved");

  declareProperty("ConfigFileVersion",           m_configFileVersion,               "BDT weight file version");
  declareProperty("ConfigPathOverride",          m_configPathOverride,              "Path of the local BDT weight file you want o study/test");
  declareProperty("InputVarDecoratePrefix",      m_inputVarDecoratePrefix,          "Prefix of the variables that will be decorated into the lepton");
  declareProperty("BDTName",                     m_BDTName,                         "BDT name");
  declareProperty("MethodTitleMVA",              m_methodTitleMVA = "BDT",          "Help to config the path of the BDT xml file");

  declareProperty("accessorRNNVars",             m_accessorRNNVars,                 "Name of the RNN accessor of the lepton");
  declareProperty("stringIntVars",               m_stringIntVars,                   "Vector of the BDT int variables' names and they will be decorated into lepton if not in the veto list");
  declareProperty("stringFloatVars",             m_stringFloatVars,                 "Vector of the BDT float variables' names and they will be decorated into lepton if not in the veto list");
  declareProperty("extraDecoratorFloatVars",     m_extraDecoratorFloatVars,         "Extra float variables' names you want to compute and decorate into the lepton");
  declareProperty("extraDecoratorShortVars",     m_extraDecoratorShortVars,         "Extra short variables' names you want to compute and decorate into the lepton");
  declareProperty("vetoDecoratorFloatVars",      m_vetoDecoratorFloatVars,          "Vector of the float variables' names you do not want to save");
  declareProperty("vetoDecoratorShortVars",      m_vetoDecoratorShortVars,          "Vector of the short variables' names you do not want to save");
  declareProperty("leptonPtBinsVector",          m_leptonPtBinsVector,              "pT bin edges that used for MVABin calculation");

  declareProperty("PrintAuxVars",                m_printAuxVars = false,            "whether print all aux variables of the lepton for debug"); 
  declareProperty("PrintTime",                   m_printTime    = false,            "whether print current time");
  
  declareProperty("VertexLinkName",              m_vertexLinkName,                  "ElementLink name of the secondary vertices");
  declareProperty("VertexMinChiSquaredProb",     m_vertexMinChiSquaredProb = 0.03,  "Vertex chi2 cut");
  declareProperty("VertexMinThetaBarrElec",      m_vertexMinThetaBarrElec  = 0.002, "Vertex theta between lepton and the direction of sv-pv cut for barrel electrons");
  declareProperty("VertexMinThetaEcapElec",      m_vertexMinThetaEcapElec  = 0.001, "Vertex theta between lepton and the direction of sv-pv cut for central electrons");
  declareProperty("VertexBarrEcapAbsEtaAt",      m_vertexBarrEcapAbsEtaAt  = 1.37,  "Relate to the vertex cut above, define the barrel and central electrons by abs(eta)");
  
  declareProperty("ElecMinCalErelConeSize",      m_elecMinCalErelConeSize  = 0.15,  "Cut of the cluster for calculate the core energy of the lepton");
  declareProperty("maxLepTrackJetDR",            m_maxLepTrackJetDR        = 0.4,   "Maximum distance between lepton and track jet for track jet matching");
}

//=============================================================================
StatusCode Prompt::DecoratePromptLeptonImproved::initialize()
{   
  if(m_printTime) {
    //
    // Reset timers
    //
    m_timerAll .Reset();
    m_timerExec.Reset();
    m_timerMuon.Reset();
    m_timerElec.Reset();

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

  m_varTMVA.resize(m_allVars.size());

  //
  // Get key for recording BDT output
  //
  m_BDTVarKey = Prompt::Def::RegisterDynamicVar(m_BDTName);

  if(m_BDTVarKey == Def::NONE) {
    ATH_MSG_ERROR("Failed to create key for BDT name=" << m_BDTName);
    return StatusCode::FAILURE;
  }

  //
  // Fill decorator maps
  //
  if(!initializeDecorators()) {
    ATH_MSG_ERROR("Failed initializeDecorator");
    return StatusCode::FAILURE;
  }

  //
  // Initialize const accessors
  //
  initializeConstAccessors();

  //
  // Initialize TMVA Reader 
  //
  return initializeTMVAReader();
}

//=============================================================================
StatusCode Prompt::DecoratePromptLeptonImproved::finalize()
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
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
StatusCode Prompt::DecoratePromptLeptonImproved::execute()
{  

  //
  // Start execute timer
  //
  TimerScopeHelper timer(m_timerExec);

  //
  // Retrieve containers from evtStore
  //
  const xAOD::JetContainer         *trackJets = nullptr;
  const xAOD::VertexContainer      *vertices  = nullptr;
  const xAOD::CaloClusterContainer *clusters  = nullptr; 
  
  CHECK(evtStore()->retrieve(trackJets, m_trackJetsName)); 
  CHECK(evtStore()->retrieve(vertices,  m_primaryVertexName)); 
  CHECK(evtStore()->retrieve(clusters,  m_clusterContainerName)); 
  
  ATH_MSG_DEBUG("=======================================" 
		<< "\n\t\t\t  Size of vertex    container: " << vertices ->size() 
		<< "\n\t\t\t  Size of track jet container: " << trackJets->size()
		<< "\n-----------------------------------------------------------------");
  
  //
  // Find default Primary Vertex
  //
  const xAOD::Vertex *primaryVertex = nullptr;

  for(const xAOD::Vertex *vertex: *vertices) {
    if(vertex->vertexType() == 1) {
      primaryVertex = vertex;
      break;
    } 
  }
 
  if(m_leptonsName == "Electrons") {
    //
    // Process electrons
    //
    const xAOD::ElectronContainer *electrons = nullptr;

    CHECK(evtStore()->retrieve(electrons, m_leptonsName)); 

    for(const xAOD::Electron *elec: *electrons) {
      decorateElec(*elec, *trackJets, *clusters, primaryVertex);
      
      if(m_printAuxVars) {
	printAuxVars(elec);
      }
    }  
  }

  else if(m_leptonsName == "Muons") {     
    //
    // Process muons
    //
    const xAOD::MuonContainer *muons = nullptr;

    CHECK(evtStore()->retrieve(muons, m_leptonsName)); 

    for(const xAOD::Muon *muon: *muons) {
      decorateMuon(*muon, *trackJets, primaryVertex);

      if(m_printAuxVars) {
	printAuxVars(muon);
      }
    }  
  }

  else {
    ATH_MSG_ERROR("Must specify Electrons or Muons");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
bool Prompt::DecoratePromptLeptonImproved::initializeDecorators()
{   
  //
  // Fill short variable map
  //
  for(Prompt::Def::Var &var: m_intVars) {
    SG::AuxElement::Decorator<short> shortDecorator(m_inputVarDecoratePrefix + Prompt::Def::AsStr(var));

    if(!m_shortMap.insert(shortDecoratorMap::value_type(var, shortDecorator)).second) {
      ATH_MSG_WARNING("Instantiation of Decorator class failed for short decorator map for var: " << Def::AsStr(var));
      return false;
    }
  }

  //
  // Fill float variable map
  //
  for(Prompt::Def::Var &var: m_floatVars) {
    SG::AuxElement::Decorator<float> floatDecorator(m_inputVarDecoratePrefix + Prompt::Def::AsStr(var));

    if(!m_floatMap.insert(floatDecoratorMap::value_type(var, floatDecorator)).second) {
      ATH_MSG_WARNING("Instantiation of Decorator class failed for float decorator map for var: " << Def::AsStr(var));
      return false;
    }
  }

  //
  // Fill additional variables
  //
  if(!m_floatMap.insert(floatDecoratorMap::value_type(m_BDTVarKey, SG::AuxElement::Decorator<float>(m_BDTName))).second) {
    ATH_MSG_WARNING("Failed to add variable: " << m_BDTVarKey); 
    return false;
  }

  for(const std::string &evar: m_extraDecoratorFloatVars) {
    const Def::Var ekey = Prompt::Def::RegisterDynamicVar(evar);
    
    if(ekey == Def::NONE) {
      ATH_MSG_WARNING("Failed to create key for variable name=" << evar);
      return false;
    }

    if(m_floatMap.find(ekey) != m_floatMap.end()) {
      ATH_MSG_DEBUG("Ignore duplicate variable name=" << evar);
      continue;
    }

    if(!m_floatMap.insert(floatDecoratorMap::value_type(ekey, SG::AuxElement::Decorator<float>(m_inputVarDecoratePrefix + evar))).second) {
      ATH_MSG_WARNING("Failed to add variable: \"" << evar << "\"");
      return false;
    }
  }

  for(const std::string &evar: m_extraDecoratorShortVars) {
    const Def::Var ekey = Prompt::Def::RegisterDynamicVar(evar);
    
    if(ekey == Def::NONE) {
      ATH_MSG_WARNING("Failed to create key for variable name=" << evar);
      return false;
    }

    if(m_shortMap.find(ekey) != m_shortMap.end()) {
      ATH_MSG_DEBUG("Ignore duplicate variable name=" << evar);
      continue;
    }

    if(!m_shortMap.insert(shortDecoratorMap::value_type(ekey, SG::AuxElement::Decorator<short>(m_inputVarDecoratePrefix + evar))).second) {
      ATH_MSG_WARNING("Failed to add variable: \"" << evar << "\"");
      return false;
    }
  }

  //
  // Veto the decorators of the variables in the veto-list if exist
  //
  for(const std::string &vvar: m_vetoDecoratorFloatVars) {
    const Def::Var vkey = Prompt::Def::RegisterDynamicVar(vvar);

    if(vkey == Def::NONE) {
      ATH_MSG_WARNING("Failed to create key for variable name=" << vvar);
      return false;
    }

    floatDecoratorMap::iterator iter = m_floatMap.find(vkey);

    if(iter != m_floatMap.end()) {
      ATH_MSG_DEBUG("Remove the variable from the veto-list, name=" << vvar);
      m_floatMap.erase(iter);
    }
  }

  for(const std::string &vvar: m_vetoDecoratorShortVars) {
    const Def::Var vkey = Prompt::Def::RegisterDynamicVar(vvar);

    if(vkey == Def::NONE) {
      ATH_MSG_WARNING("Failed to create key for variable name=" << vvar);
      return false;
    }

    shortDecoratorMap::iterator iter = m_shortMap.find(vkey);

    if(iter != m_shortMap.end()) {
      ATH_MSG_DEBUG("Remove the variable from the veto-list, name=" << vvar);
      m_shortMap.erase(iter);
    }
  }
  
  //
  // Print all decorators
  //
  static SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
    
  ATH_MSG_INFO("Added " << m_shortMap.size() << " short decorators");

  for(const shortDecoratorMap::value_type &dec: m_shortMap) {
    ATH_MSG_INFO("  short decorator name=\"" << reg.getName(dec.second.auxid()) << "\", for var=\"" << Def::AsStr(dec.first) << "\"");
  }

  ATH_MSG_INFO("Added " << m_floatMap.size() << " float decorators");

  for(const floatDecoratorMap::value_type &dec: m_floatMap) {
    ATH_MSG_INFO("  float decorator name=\"" << reg.getName(dec.second.auxid()) << "\", for var=\"" << Def::AsStr(dec.first) << "\"");
  }
  
  //
  // Instantiate MVA X bin  
  //
  if(m_leptonPtBinsVector.size() < 2) {
    ATH_MSG_WARNING("Invalid PtBins size=" << m_leptonPtBinsVector.size());
    return false;
  }

  std::unique_ptr<double []> PtBins = std::make_unique<double []>(m_leptonPtBinsVector.size());

  for(unsigned i = 0; i < m_leptonPtBinsVector.size(); i++) {
    PtBins[i] = m_leptonPtBinsVector.at(i);
  }

  m_leptonPtBinHist = std::make_unique<TH1D>("PtBin", "PtBin", m_leptonPtBinsVector.size() - 1, PtBins.get());

  return true;
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::initializeConstAccessors()
{   
  //
  // Instantiate isolation accessors
  //
  m_accessCalIsolation30       = std::make_unique<AccessFloat> ("topoetcone30");
  m_accessTrackIsolation30     = std::make_unique<AccessFloat> ("ptvarcone30");
  m_accessTrackIsolation30TTVA = std::make_unique<AccessFloat> ("ptvarcone30_TightTTVA_pt500");

  m_accessDeepSecondaryVertex  = std::make_unique<AccessVertex>(m_vertexLinkName);
  
  //
  // Instantiate accessors for the muon specific variables
  //
  m_accessMuonCalE            = std::make_unique<AccessFloat> ("calE");
  m_accessMuonParamEnergyLoss = std::make_unique<AccessFloat> ("ParamEnergyLoss");
 
  //
  // Instantiate accessors for RNN variables
  // 
  for(const std::string &name: m_accessorRNNVars) {
    const Def::Var akey = Prompt::Def::RegisterDynamicVar(name);
    
    if(m_accessRNNMap.insert(floatAccessorMap::value_type(akey, AccessFloat(name))).second) {
      ATH_MSG_INFO("Add float RNN accessor: " << name);
    }
    else {
      ATH_MSG_WARNING("Skip duplicate float accessor: " << name);
    }
  }
}

//=============================================================================
StatusCode Prompt::DecoratePromptLeptonImproved::initializeTMVAReader()
{
  //
  // Get path to xml training file
  //
  std::string fullPathToFile;
 
  if(!m_configPathOverride.empty()) {
    ATH_MSG_INFO("Override PathResolver to this path: " << m_configPathOverride);
    fullPathToFile = m_configPathOverride;
  }
  else {
    fullPathToFile = PathResolverFindCalibFile("JetTagNonPromptLepton/" 
					       + m_configFileVersion 
					       + "/TMVAClassification_" + m_methodTitleMVA + ".weights.xml");
  }

  ATH_MSG_INFO("initializeTMVAReader - TMVA configuration file: " + fullPathToFile);
  ATH_MSG_INFO("initializeTMVAReader - TMVA method name: " + m_methodTitleMVA);

  //
  // Book an instance of BDT reader if path is available
  //
  struct stat buffer; 

  if(stat(fullPathToFile.c_str(), &buffer) != 0) {
    ATH_MSG_ERROR("Invalid path to xml training file: " << fullPathToFile);
    return StatusCode::FAILURE;
  }

  //
  // Make new instance of TMVA Reader and add variables
  //
  m_TMVAReader = std::make_unique<TMVA::Reader> ();

  for(unsigned i = 0; i < m_allVars.size(); ++i) { 
    m_TMVAReader->AddVariable(Prompt::Def::AsStr(m_allVars.at(i)), &m_varTMVA[i]);   
  }
  
  m_TMVAReader->BookMVA(m_methodTitleMVA, fullPathToFile);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::decorateElec(const xAOD::Electron             &electron, 
							const xAOD::JetContainer         &trackJets,
							const xAOD::CaloClusterContainer &clusters,
							const xAOD::Vertex               *primaryVertex)
{ 
  //
  // Find nearest track jet to electron
  //
  TimerScopeHelper timer(m_timerElec);

  Prompt::VarHolder vars;

  const xAOD::Jet *track_jet = FindTrackJet(electron, trackJets);

  if(track_jet) {   
    //
    // Get muon calorimeter energy variable, RNN and secondary vertex variables
    //
    getElectronAnpVariables(electron, clusters, vars, primaryVertex);

    //
    // Get mutual variables, passing track as argument 
    // 
    getMutualVariables(electron, *track_jet, electron.trackParticle(), vars);

    //
    // Pass variables to TMVA
    //
    addVarsToTMVA(vars);
  }
  else {
    //
    // Decorate electron with default values
    //
    fillVarDefault(vars);

    ATH_MSG_DEBUG("No track jet found near to electron");
  }

  //
  // Decorate electron with input vars and BDT weight
  //
  decorateAuxLepton(electron, vars);

  ATH_MSG_DEBUG("Electron BDT score: " << electron.auxdataConst<float>(m_BDTName)
		<< "\n-----------------------------------------------------------------");
}


//=============================================================================
void Prompt::DecoratePromptLeptonImproved::decorateMuon(const xAOD::Muon         &muon, 
					         	const xAOD::JetContainer &trackJets,
                                                        const xAOD::Vertex       *primaryVertex)
{ 
  //
  // Find nearest track jet to muon
  //
  TimerScopeHelper timer(m_timerMuon);

  Prompt::VarHolder vars;

  const xAOD::Jet *track_jet = FindTrackJet(muon, trackJets);

  if(track_jet) {
    //
    // Get muon calorimeter energy variable, RNN and secondary vertex variables
    //
    getMuonAnpVariables(muon, vars, primaryVertex);
    
    //
    // Get mutual variables, passing track as argument 
    //
    getMutualVariables(muon, *track_jet, muon.primaryTrackParticle(), vars);

    //
    // Add variables to TMVA Reader
    //
    addVarsToTMVA(vars);
  }
  else {
    //
    // Decorate muon with default values
    //
    fillVarDefault(vars);

    ATH_MSG_DEBUG("No track jet found near to muon");
  }
    
  //
  // Decorate muon with input vars and BDT weight
  //
  decorateAuxLepton(muon, vars);

  ATH_MSG_DEBUG("Muon BDT score: " << muon.auxdataConst<float>(m_BDTName) 
		<< "\n-----------------------------------------------------------------");
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::getElectronAnpVariables(const xAOD::Electron             &elec, 
                                                                   const xAOD::CaloClusterContainer &clusters,
						                   Prompt::VarHolder                &vars,
                                                                   const xAOD::Vertex               *primaryVertex)
{
  //
  // Get Muon variables - calorimeter
  //
  double sumCoreEt_large = 0.0, CaloClusterSumEtRel = 0.0;
  
  if(elec.caloCluster()) {
    const double elec_calEta = elec.caloCluster()->eta(); 
    const double elec_calPhi = elec.caloCluster()->phi();
    
    for(const xAOD::CaloCluster *cluster: clusters) {
      const double deta =                      elec_calEta - cluster->eta();
      const double dphi = TVector2::Phi_mpi_pi(elec_calPhi - cluster->phi());
      const double dr   = std::sqrt(deta*deta + dphi*dphi);

      if(dr < m_elecMinCalErelConeSize) {
        sumCoreEt_large += cluster->pt();
      }
    } 
  }  
  
  if(elec.pt() > 0.0) CaloClusterSumEtRel = sumCoreEt_large/elec.pt();
  
  vars.AddVar(Prompt::Def::CaloClusterSumEtRel, CaloClusterSumEtRel);

  //
  // Get lepton isolation variables 
  //
  const double topoetcone30rel = accessIsolation(*m_accessCalIsolation30,   elec);
  const double ptvarcone30rel  = accessIsolation(*m_accessTrackIsolation30, elec);

  vars.AddVar(Prompt::Def::topoetcone30rel, topoetcone30rel);  
  vars.AddVar(Prompt::Def::ptvarcone30rel,  ptvarcone30rel); 

  //
  // Get scondary vertex variable 
  //
  std::vector<double> good_vertex_ndist_long;

  if(m_accessDeepSecondaryVertex->isAvailable(elec)) {
    std::vector<ElementLink<xAOD::VertexContainer> > vtxLinks = (*m_accessDeepSecondaryVertex)(elec);
    
    for(ElementLink<xAOD::VertexContainer> &vtxLink: vtxLinks) {      
      if(!vtxLink.isValid()) {
	ATH_MSG_WARNING("VertexContainer : invalid link");	
	continue;
      }
      
      const xAOD::Vertex *vtx = *vtxLink;
        
      const double fit_prob = Prompt::GetVertexFitProb(vtx);
	
      if(fit_prob < m_vertexMinChiSquaredProb) {
	continue;
      }
      
      const double theta = std::acos(GetVertexCosThetaWithLepDir(elec, vtx, primaryVertex));
      
      if      (theta < m_vertexMinThetaBarrElec && std::fabs(elec.eta()) <= m_vertexBarrEcapAbsEtaAt) continue;
      else if (theta < m_vertexMinThetaEcapElec && std::fabs(elec.eta()) >  m_vertexBarrEcapAbsEtaAt) continue;
      
      const double vertex_ndist_long = GetVertexLongitudinalNormDist(elec, vtx, primaryVertex);

      good_vertex_ndist_long.push_back(vertex_ndist_long);
    }
  }
  else {
    ATH_MSG_WARNING("VertexContainer : " << m_vertexLinkName << " not found for the electron");
  }

  double best_vertex_ndist_long = 0.0;
  
  if(good_vertex_ndist_long.size() > 0) { 
    std::sort(good_vertex_ndist_long.begin(), good_vertex_ndist_long.end());
    best_vertex_ndist_long = good_vertex_ndist_long.back();
  }

  vars.AddVar(Prompt::Def::CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx, best_vertex_ndist_long);
  vars.AddVar(Prompt::Def::CandVertex_NPassVtx,                         good_vertex_ndist_long.size());
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::getMuonAnpVariables(const xAOD::Muon   &muon, 
						               Prompt::VarHolder  &vars,
                                                               const xAOD::Vertex *primaryVertex)
{
  //
  // Get Muon variables - calorimeter
  //
  double calE = -99.0, peloss = -99.0, CaloClusterERel = -99.0;

  if(muon.clusterLink().isValid()) {
    const xAOD::CaloCluster* cluster = *(muon.clusterLink());

    if(m_accessMuonCalE->isAvailable(*cluster) && m_accessMuonParamEnergyLoss->isAvailable(muon)) {
      calE   = (*m_accessMuonCalE)(*cluster);
      peloss = (*m_accessMuonParamEnergyLoss)(muon);

      CaloClusterERel = calE/peloss;
    }
    else {
      ATH_MSG_WARNING("Muon calE or ParamEnergyLoss not found in auxiliary store");   
    }
  }

  vars.AddVar(Prompt::Def::CaloClusterERel, CaloClusterERel);
 
  //
  // Get lepton isolation variables 
  //
  const double topoetcone30rel              = accessIsolation(*m_accessCalIsolation30,       muon);
  const double ptvarcone30TightTTVAPt500rel = accessIsolation(*m_accessTrackIsolation30TTVA, muon);

  vars.AddVar(Prompt::Def::topoetcone30rel,              topoetcone30rel);
  vars.AddVar(Prompt::Def::ptvarcone30_TightTTVA_pt500rel, ptvarcone30TightTTVAPt500rel);

  //
  // Get Muon Secondary Vertex variable 
  //
  std::vector<double> good_vertex_ndist_long;

  if(m_accessDeepSecondaryVertex->isAvailable(muon)) {
    std::vector<ElementLink<xAOD::VertexContainer> > vtxLinks = (*m_accessDeepSecondaryVertex)(muon);
    good_vertex_ndist_long.reserve(vtxLinks.size());
     
    for(ElementLink<xAOD::VertexContainer> &vtxLink: vtxLinks) {
      if(!vtxLink.isValid()) {
	ATH_MSG_WARNING("VertexContainer : invalid link");	
	continue;
      }

      const xAOD::Vertex *vtx = *vtxLink;
      
      const double fit_prob = Prompt::GetVertexFitProb(vtx);
      
      if(fit_prob > m_vertexMinChiSquaredProb) {
	const double vertex_ndist_long = GetVertexLongitudinalNormDist(muon, vtx, primaryVertex);
	
	good_vertex_ndist_long.push_back(vertex_ndist_long);
      }
    }
  }
  else {
    ATH_MSG_WARNING("VertexContainer : " << m_vertexLinkName << " not found for the muon");
  }

  double best_vertex_ndist_long = 0.0;
  
  if(good_vertex_ndist_long.size() > 0) { 
    std::sort(good_vertex_ndist_long.begin(), good_vertex_ndist_long.end());
    best_vertex_ndist_long = good_vertex_ndist_long.back();
  }
  
  vars.AddVar(Prompt::Def::CandVertex_normDistToPriVtxLongitudinalBest, best_vertex_ndist_long);
  vars.AddVar(Prompt::Def::CandVertex_NPassVtx,                         good_vertex_ndist_long.size());
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::getMutualVariables(const xAOD::IParticle     &particle,
						              const xAOD::Jet           &track_jet, 
						              const xAOD::TrackParticle *track,
						              Prompt::VarHolder         &vars)
{
  //
  // Add lepton - jet variables to VarHolder
  //
  double PtFrac = -99.;
  double PtRel  = -99.;
 
  if(particle.pt() > 0.0 && track_jet.pt() > 0.0) {

    if(track) {
      PtFrac = track->pt()/track_jet.pt();
    }

    const double angle = particle.p4().Vect().Angle(track_jet.p4().Vect());

    PtRel = particle.pt() * std::sin(angle);
  }

  //
  // Add vars to VarHolder
  //
  vars.AddVar(Prompt::Def::PtFrac,         PtFrac);
  vars.AddVar(Prompt::Def::PtRel,          PtRel);
  vars.AddVar(Prompt::Def::DRlj,           track_jet.p4().DeltaR(particle.p4()));
  vars.AddVar(Prompt::Def::TrackJetNTrack, track_jet.getConstituents().size());

  //
  // Get RNN variables
  //
  for(floatAccessorMap::value_type &acc: m_accessRNNMap) {
    if(acc.second.isAvailable(particle)) {
      vars.AddVar(acc.first, acc.second(particle));
    }
    else {
      ATH_MSG_WARNING("LeptonTagger RNN not found in auxiliary store for variable=" << Def::AsStr(acc.first));
    }
  }

  //
  // Get lepton variables - pT X bin 
  //
  const double lepPt = particle.pt();
  
  const double xmax = m_leptonPtBinHist->GetXaxis()->GetXmax();
  const double xmin = m_leptonPtBinHist->GetXaxis()->GetXmin();

  int curr_bin = 0;

  if(xmin < lepPt && lepPt < xmax) {
    curr_bin = m_leptonPtBinHist->FindBin(lepPt);
  }   
  else if (!(lepPt < xmax)) {
    curr_bin = m_leptonPtBinHist->GetNbinsX();
  }   
  else if (!(lepPt > xmin)) {
    curr_bin = 1;
  }   

  vars.AddVar(Prompt::Def::MVAXBin, curr_bin);
  vars.AddVar(Prompt::Def::RawPt,   lepPt);

  ATH_MSG_DEBUG("getMutualVariables - lepPt = " << lepPt << ", MVAXBin = " << curr_bin); 
}

//=============================================================================
float Prompt::DecoratePromptLeptonImproved::accessIsolation(AccessFloat           &isoAccessor, 
						            const xAOD::IParticle &particle)
{
  double isolation = -99., isolationrel = -99.;
  
  if(isoAccessor.isAvailable(particle)) {
    isolation = isoAccessor(particle);
  }
  else {
    ATH_MSG_WARNING("Lepton isolation not found in auxiliary store");
  }

  if(particle.pt() > 0.0) {
    isolationrel = isolation / particle.pt();
  }

  return isolationrel;
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::addVarsToTMVA(Prompt::VarHolder &vars)
{
  //
  // Add variables to TMVA reader
  //
  for(unsigned i = 0; i < m_allVars.size(); ++i) {  
    m_varTMVA[i] = 0.0;

    if(!vars.GetVar(m_allVars.at(i), m_varTMVA[i])) {
      ATH_MSG_WARNING("Missing input variable: " << Prompt::Def::AsStr(m_allVars.at(i)));   
    }
  }

  //
  // Decorate lepton with classifier response, if goodJet
  //
  const double bdt_weight = m_TMVAReader->EvaluateMVA(m_methodTitleMVA);
    
  if(m_BDTVarKey != Def::NONE) {
    vars.AddVar(m_BDTVarKey, bdt_weight);
  }
  else {
    ATH_MSG_WARNING("addVarsToTMVA - invalid Def::Var key for " << m_BDTName);
  }
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::fillVarDefault(Prompt::VarHolder &vars) const
{
  //
  // Add default values to VarHolder
  //
  for(const floatDecoratorMap::value_type &dec: m_floatMap) {
    vars.AddVar(dec.first, -99.0);
  }

  for(const shortDecoratorMap::value_type &dec: m_shortMap) {
    vars.AddVar(dec.first, -99.0);
  }
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::decorateAuxLepton(const xAOD::IParticle   &particle, 
							     const Prompt::VarHolder &vars)
{
  //
  // Decorate lepton with input short variables
  //
  for(shortDecoratorMap::value_type &dec: m_shortMap) {
    double val = 0.0;

    if(vars.GetVar(dec.first, val)) {
      dec.second(particle) = static_cast<short>(val);

      ATH_MSG_DEBUG("Short variable: " << Prompt::Def::AsStr(dec.first) << " = " << val);
    }    
    else {
      ATH_MSG_WARNING("Variable " << Prompt::Def::AsStr(dec.first) << " not decorated to lepton");
    }
  }  

  //
  // Decorate lepton with input float variables
  //
  for(floatDecoratorMap::value_type &dec: m_floatMap) {
    double val = 0.0;

    if(vars.GetVar(dec.first, val)) {
      dec.second(particle) = val;

      ATH_MSG_DEBUG("Float variable: " << Prompt::Def::AsStr(dec.first) << " = " << val);
    }
    else {
      ATH_MSG_WARNING("Variable " << Prompt::Def::AsStr(dec.first) << " not decorated to lepton");
    }
  }
}

//=============================================================================
template<class T> const xAOD::Jet* Prompt::DecoratePromptLeptonImproved::FindTrackJet(const T &part, 
										      const xAOD::JetContainer &jets)
{
  //
  // Find nearest track jet and a return a pair of dR and xAOD::Jet*
  //
  const xAOD::Jet *minjet = 0;  
  double           mindr  = 10.0;
	
  for(const xAOD::Jet* jet: jets) {
    const double dr = part.p4().DeltaR(jet->p4());
    
    if(minjet) {
      if(dr < mindr) {
	mindr  = dr;
	minjet = jet;
      }
    }
    else {
      mindr  = dr;
      minjet = jet;
    }
  }


  if(minjet && mindr < m_maxLepTrackJetDR) {
    return minjet;
  }

  return 0;
}

//=============================================================================
void Prompt::DecoratePromptLeptonImproved::printAuxVars(const xAOD::IParticle* particle) const
{
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

//=============================================================================
double Prompt::DecoratePromptLeptonImproved::GetVertexLongitudinalNormDist(const xAOD::IParticle &lepton,
                                                                           const xAOD::Vertex    *secondaryVertex,
                                                                           const xAOD::Vertex    *primaryVertex)
{
  //
  // get the Longitudinal nomalized distance between the secondary vertex and primary vertex
  //
  if(!secondaryVertex || !primaryVertex) {
    ATH_MSG_WARNING("GetVertexLongitudinalNormDist - invalid pointer of lepton/secondaryVertex/primaryVertex");
    return 0.0;
  } 


  float normDist_SVPV  = 0.0;

  if(!Prompt::GetAuxVar(*secondaryVertex, normDist_SVPV,  "normDistToPriVtx")) {
    ATH_MSG_WARNING("GetVertexLongitudinalNormDist - missing \"normDistToPriVtx\"");    
  }

  double cos_theta = GetVertexCosThetaWithLepDir(lepton, secondaryVertex, primaryVertex);

  return normDist_SVPV*cos_theta;
}

//=============================================================================
double Prompt::DecoratePromptLeptonImproved::GetVertexCosThetaWithLepDir(const xAOD::IParticle &lepton,
                                                                         const xAOD::Vertex    *secondaryVertex,
                                                                         const xAOD::Vertex    *primaryVertex)
{
  //
  // get the Longitudinal nomalized distance between the secondary vertex and primary vertex
  //
  if(!secondaryVertex || !primaryVertex) {
    ATH_MSG_WARNING("GetVertexThetaWithLepDir - invalid pointer of lepton/secondaryVertex/primaryVertex");
    return 0.0;
  } 
 
  const Amg::Vector3D sv_to_pv_v3 = secondaryVertex->position() - primaryVertex->position();

  const TVector3 sv_to_pv_t3 = TVector3(sv_to_pv_v3.x(), sv_to_pv_v3.y(), sv_to_pv_v3.z());
  const TVector3 lepton_dirt = lepton.p4().Vect();

  const double cos_theta = sv_to_pv_t3.Unit()*lepton_dirt.Unit();

  return cos_theta;
}

