/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetReclustering/JetReclusteringTool.h"
#include "AsgTools/Check.h"

#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "JetInterface/IJetFinder.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetInterface/IJetPseudojetRetriever.h"

#include <sstream>

#ifdef ROOTCORE // The macro for RootCore needs these (provides a way of getting around missing dictionaries)
#include "JetRec/JetFilterTool.h"
#include "JetRec/JetRecTool.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetTrimmer.h"
#include "JetRec/JetRecursiveSoftDrop.h"
#include "JetRec/JetPseudojetRetriever.h"
#ifndef XAOD_STANDALONE
#include "JetRecTools/TrackPseudoJetGetter.h"
#endif
#include "JetReclustering/EffectiveRTool.h"
#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureMomentTools/JetPullTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedRatiosTool.h"
#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#endif

JetReclusteringTool::JetReclusteringTool(std::string name) :
  AsgTool(name),
  m_jetFilterTool("JetFilterTool/JetFilterTool_" + this->name()),
  m_inputJetFilterTool("JetRecTool/JetRec_InputJetFilterTool_" + this->name()),
  m_pseudoJetGetterTool("PseudoJetGetter/PseudoJetGetterTool_" + this->name()),
  m_pseudoGhostTrackJetGetterTool("TrackPseudoJetGetter/PseudoGhostTrackJetGetterTool_" + this->name()),
  m_jetFromPseudoJetTool("JetFromPseudojet/JetFromPseudoJetTool_" + this->name()),
  m_jetFinderTool("JetFinder/JetFinderTool_" + this->name()),
  m_reclusterJetTool("JetRecTool/JetRec_JetReclusterTool_" + this->name()),
  m_groomJetTool("JetRecTool/JetRec_JetGroomTool_" + this->name()),
  m_effectiveRTool("EffectiveRTool/EffectiveRTool"),
  m_jetGroomingTool(""),
  m_jetGroomTool_JPJR("JetPseudojetRetriever/JetRec_JetGroomTool_JPJR_" + this->name()),
  m_jetChargeTool("JetChargeTool/JetChargeTool_" + this->name()),
  m_jetPullTool("JetPullTool/JetPullTool_" + this->name()),
  m_energyCorrelatorTool("EnergyCorrelatorTool/EnergyCorrelatorTool_" + this->name()),
  m_energyCorrelatorRatiosTool("EnergyCorrelatorRatiosTool/EnergyCorrelatorRatiosTool_" + this->name()),
  m_energyCorrelatorGeneralizedTool("EnergyCorrelatorGeneralizedTool/EnergyCorrelatorGeneralizedTool_" + this->name()),
  m_energyCorrelatorGeneralizedRatiosTool("EnergyCorrelatorGeneralizedRatiosTool/EnergyCorrelatorGeneralizedRatiosTool_" + this->name()),
  m_ktSplittingScaleTool("KTSplittingScaleTool/KTSplittingScaleTool_" + this->name()),
  m_dipolarityTool("DipolarityTool/DipolarityTool_" + this->name()),
  m_centerOfMassShapesTool("CenterOfMassShapesTool/CenterOfMassShapesTool_" + this->name()),
  m_nSubjettinessTool("NSubjettinessTool/NSubjettinessTool_" + this->name()),
  m_pseudoBTagJetGetterTool("PseudoJetGetter/PseudoJetGetterTool_BTagJet_" + this->name()),
  m_pseudoTruthBQuarksGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthBQuarks_" + this->name()),
  m_pseudoTruthBHadronsGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthBHadrons_" + this->name()),
  m_pseudoTruthCQuarksGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthCQuarks_" + this->name()),
  m_pseudoTruthCHadronsGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthCHadrons_" + this->name()),
  m_pseudoTruthTQuarksGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthTQuarks_" + this->name()),
  m_pseudoTruthWBosonsGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthWBosons_" + this->name()),
  m_pseudoTruthZBosonsGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthZBosons_" + this->name()),
  m_pseudoTruthHBosonsGetterTool("PseudoJetGetter/PseudoJetGetterTool_TruthHBosons_" + this->name())
{


  declareProperty("InputJetContainer",             m_inputJetContainer = "");
  declareProperty("OutputJetContainer",            m_outputJetContainer = "");
  declareProperty("OutputUngroomedJetContainer",   m_ungroomedJetContainer = "");
  declareProperty("ReclusterRadius",               m_radius = 1.0);
  declareProperty("ReclusterAlgorithm",            m_rc_alg = "AntiKt");
  declareProperty("VariableRMinRadius",            m_varR_minR =-1.0);
  declareProperty("VariableRMassScale",            m_varR_mass =-1.0);
  declareProperty("InputJetPtMin",                 m_ptMin_input = 25.0);
  declareProperty("RCJetPtMin",                    m_ptMin_rc = 50.0);
  declareProperty("GroomingAlg",                   m_groomAlg = "trim");
  declareProperty("TrimPtFrac",                    m_trim_ptFrac = 0.05); // Was previously RCJetPtFrac
  declareProperty("TrimSubjetRadius",              m_trim_subjet_radius = 0.01); // Was previously RCJetSubjetRadius
  declareProperty("SoftDropZCut",                  m_sd_zcut = 0.1);
  declareProperty("SoftDropBeta",                  m_sd_beta = 0.0);
  declareProperty("SoftDropR0",                    m_sd_R0 = 1.0);
  declareProperty("SoftDropN",                     m_sd_N = 1);
  declareProperty("DoArea",                        m_doArea = false);
  declareProperty("AreaAttributes",                m_areaAttributes = "ActiveArea ActiveArea4vec");
  declareProperty("ECFBetaList",                   m_ecf_betaVals = {});
  declareProperty("ECFDoDichroic",                 m_ecf_doDichroic = false);
  declareProperty("ECFDoC3",                       m_ecf_doC3 = false);
  declareProperty("ECFGDoN3",                      m_ecfg_doN3 = false);
  declareProperty("ECFGDoLSeries",                 m_ecfg_doLSeries = false);
  declareProperty("GhostTracksInputContainer",         m_ghostTracksInputContainer = "");
  declareProperty("GhostTracksVertexAssociationName",  m_ghostTracksVertexAssociationName = "");
  declareProperty("GhostBTagJetInputContainer",        m_ghostBTagJetInputContainer = "");
  declareProperty("GhostBTagJetLabel",                 m_ghostBTagJetLabel = "");
  declareProperty("GhostTruthBHadronsInputContainer",  m_ghostTruthBHadronsInputContainer = "");
  declareProperty("GhostTruthBHadronsLabel",           m_ghostTruthBHadronsLabel = "");
  declareProperty("GhostTruthCHadronsInputContainer",  m_ghostTruthCHadronsInputContainer = "");
  declareProperty("GhostTruthCHadronsLabel",           m_ghostTruthCHadronsLabel = "");
  declareProperty("GhostScale",                m_ghostScale = 1e-20);

}

StatusCode JetReclusteringTool::initialize(){
  if(m_isInitialized){
    ATH_MSG_WARNING(m_APP_NAME << " already initialized.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(m_APP_NAME << " initializing");
  ATH_MSG_INFO( "Name is " << name());


  ATH_MSG_INFO( "... with containers:" );
  ATH_MSG_INFO( "\tInputJetContainer: " << m_inputJetContainer );
  ATH_MSG_INFO( "\tOutputJetContainer: " << m_outputJetContainer );

  // make sure a supported grooming algorithm is used
  if(m_groomAlg!="trim" && m_groomAlg!="softdrop") {
    ATH_MSG_ERROR("Unrecognized grooming algorithm: " << m_groomAlg << ". Exiting");
    return StatusCode::FAILURE;
  }

  // set to true, we're calling it now
  m_isInitialized = true;

  // set up some stuff to use within the initialize()
  ToolHandleArray<IJetModifier> modArray;
  ToolHandleArray<IPseudoJetGetter> getterArray;

  // this is for intermediate output containers used between tools
  std::string filteredInputJetContainer(m_inputJetContainer);
  if(m_ptMin_input > 0) filteredInputJetContainer = "Filtered_"+name();
  std::string filteredInputPseudoJetsContainer = "PseudoJets_"+name();
  // if ungroomed jet container not configured, provide default name
  if(m_ungroomedJetContainer.empty()) m_ungroomedJetContainer = "ReclusteredJets_"+name();

  if(m_ptMin_input > 0){
    ATH_MSG_INFO( "Input Jet Filtering" );
    ATH_MSG_INFO( "\tPtMin: " << m_ptMin_input*1.e3 << " GeV" );
    /* initialize input jet filtering */
    //    - create a tool that will filter jets
    ASG_CHECK(m_jetFilterTool.setProperty("PtMin", m_ptMin_input*1.e3));
    ASG_CHECK(m_jetFilterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_jetFilterTool.retrieve() );
    modArray.clear();
    modArray.push_back(m_jetFilterTool.getHandle() );
    ATH_CHECK(modArray.retrieve() );

    //    - create the master tool to filter the input jets
    ASG_CHECK(m_inputJetFilterTool.setProperty("InputContainer", m_inputJetContainer));
    ASG_CHECK(m_inputJetFilterTool.setProperty("OutputContainer", filteredInputJetContainer));
    ASG_CHECK(m_inputJetFilterTool.setProperty("JetModifiers", modArray));
    /* note: we cannot use shallow copies since we are removing elements from a
     * container, we need a deep copy as linking will break */
    ASG_CHECK(m_inputJetFilterTool.setProperty("ShallowCopy", false));
    ASG_CHECK(m_inputJetFilterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_inputJetFilterTool.retrieve());
  }

  // only compute area if ptFrac = 0.0 and m_areaAttributes is specified
  float ghostArea(0.0);
  std::vector<std::string> areaAttributes;
  if(m_doArea){
    ATH_MSG_INFO( "Ghost Area Calculations:" );
    ATH_MSG_INFO( "\tAreaAttributes: " << m_areaAttributes );
    ghostArea = 0.01;
    // split up the m_areaAttributes string specifying which attributes to record
    std::string token;
    std::istringstream ss(m_areaAttributes);
    while(std::getline(ss, token, ' '))
      areaAttributes.push_back(token);
  }
  /* initialize jet reclustering */
  //    - create a PseudoJet builder.
  ATH_MSG_INFO( "PseudoJet Builder initializing..." );
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("InputContainer", filteredInputJetContainer));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("OutputContainer", filteredInputPseudoJetsContainer));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("Label", "LCTopo"));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("SkipNegativeEnergy", true));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("GhostScale", 0.0));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_pseudoJetGetterTool.retrieve());
  getterArray.push_back(m_pseudoJetGetterTool.getHandle());
  if(!m_ghostTracksInputContainer.empty()){
  //    - do we need ghost tracks too?
  #ifndef XAOD_STANDALONE
    ATH_MSG_INFO( "GhostTracks PseudoJet Builder initializing..." );
    if(m_ghostTracksVertexAssociationName.empty()){
      ATH_MSG_ERROR( "You must set the GhostTracksVertexAssoctiationName as well!" );
      return StatusCode::FAILURE;
    }
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.setProperty("InputContainer", m_ghostTracksInputContainer));
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.setProperty("OutputContainer", "GhostTracks"+name()));
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.setProperty("Label", "GhostTrack"));
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.setProperty("TrackVertexAssociation", m_ghostTracksVertexAssociationName));
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoGhostTrackJetGetterTool.retrieve());
    getterArray.push_back(m_pseudoGhostTrackJetGetterTool.getHandle());
  #else
    ATH_MSG_ERROR( "Ghost track association is ATHENA ONLY, setting the option is doing nothing." );
    return StatusCode::FAILURE;
  #endif
  }
  if(!m_ghostBTagJetInputContainer.empty()){
    ATH_MSG_INFO( "GhostBTagJets PseudoJet Builder initializing..." );
    if(m_ghostBTagJetLabel.empty()) m_ghostBTagJetLabel = "Ghost" + m_ghostBTagJetInputContainer;
    ASG_CHECK(m_pseudoBTagJetGetterTool.setProperty("InputContainer",  m_ghostBTagJetInputContainer));
    ASG_CHECK(m_pseudoBTagJetGetterTool.setProperty("OutputContainer", "GhostBTagJets"+name()));
    ASG_CHECK(m_pseudoBTagJetGetterTool.setProperty("Label", m_ghostBTagJetLabel));
    ASG_CHECK(m_pseudoBTagJetGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoBTagJetGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoBTagJetGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoBTagJetGetterTool.retrieve());
    getterArray.push_back(m_pseudoBTagJetGetterTool.getHandle());
  }
  if(!m_ghostTruthBQuarksInputContainer.empty()){
    ATH_MSG_INFO( "GhostBQuarks PseudoJet Builder initializing..." );
    if(m_ghostTruthBQuarksLabel.empty()) m_ghostTruthBQuarksLabel = "Ghost" + m_ghostTruthBQuarksInputContainer;
    ASG_CHECK(m_pseudoTruthBQuarksGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthBQuarksInputContainer));
    ASG_CHECK(m_pseudoTruthBQuarksGetterTool.setProperty("OutputContainer", "GhostTruthBQuarks"+name()));
    ASG_CHECK(m_pseudoTruthBQuarksGetterTool.setProperty("Label", m_ghostTruthBQuarksLabel));
    ASG_CHECK(m_pseudoTruthBQuarksGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthBQuarksGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthBQuarksGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthBQuarksGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthBQuarksGetterTool.getHandle());
  }
  if(!m_ghostTruthBHadronsInputContainer.empty()){
    ATH_MSG_INFO( "GhostBHadrons PseudoJet Builder initializing..." );
    if(m_ghostTruthBHadronsLabel.empty()) m_ghostTruthBHadronsLabel = "Ghost" + m_ghostTruthBHadronsInputContainer;
    ASG_CHECK(m_pseudoTruthBHadronsGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthBHadronsInputContainer));
    ASG_CHECK(m_pseudoTruthBHadronsGetterTool.setProperty("OutputContainer", "GhostTruthBHadrons"+name()));
    ASG_CHECK(m_pseudoTruthBHadronsGetterTool.setProperty("Label", m_ghostTruthBHadronsLabel));
    ASG_CHECK(m_pseudoTruthBHadronsGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthBHadronsGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthBHadronsGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthBHadronsGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthBHadronsGetterTool.getHandle());
  }
  if(!m_ghostTruthCQuarksInputContainer.empty()){
    ATH_MSG_INFO( "GhostCQuarks PseudoJet Builder initializing..." );
    if(m_ghostTruthCQuarksLabel.empty()) m_ghostTruthCQuarksLabel = "Ghost" + m_ghostTruthCQuarksInputContainer;
    ASG_CHECK(m_pseudoTruthCQuarksGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthCQuarksInputContainer));
    ASG_CHECK(m_pseudoTruthCQuarksGetterTool.setProperty("OutputContainer", "GhostTruthCQuarks"+name()));
    ASG_CHECK(m_pseudoTruthCQuarksGetterTool.setProperty("Label", m_ghostTruthCQuarksLabel));
    ASG_CHECK(m_pseudoTruthCQuarksGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthCQuarksGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthCQuarksGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthCQuarksGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthCQuarksGetterTool.getHandle());
  }
  if(!m_ghostTruthCHadronsInputContainer.empty()){
    ATH_MSG_INFO( "GhostCHadrons PseudoJet Builder initializing..." );
    if(m_ghostTruthCHadronsLabel.empty()) m_ghostTruthCHadronsLabel = "Ghost" + m_ghostTruthCHadronsInputContainer;
    ASG_CHECK(m_pseudoTruthCHadronsGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthCHadronsInputContainer));
    ASG_CHECK(m_pseudoTruthCHadronsGetterTool.setProperty("OutputContainer", "GhostTruthCHadrons"+name()));
    ASG_CHECK(m_pseudoTruthCHadronsGetterTool.setProperty("Label", m_ghostTruthCHadronsLabel));
    ASG_CHECK(m_pseudoTruthCHadronsGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthCHadronsGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthCHadronsGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthCHadronsGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthCHadronsGetterTool.getHandle());
  }
  if(!m_ghostTruthTQuarksInputContainer.empty()){
    ATH_MSG_INFO( "GhostTQuarks PseudoJet Builder initializing..." );
    if(m_ghostTruthTQuarksLabel.empty()) m_ghostTruthTQuarksLabel = "Ghost" + m_ghostTruthTQuarksInputContainer;
    ASG_CHECK(m_pseudoTruthTQuarksGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthTQuarksInputContainer));
    ASG_CHECK(m_pseudoTruthTQuarksGetterTool.setProperty("OutputContainer", "GhostTruthTQuarks"+name()));
    ASG_CHECK(m_pseudoTruthTQuarksGetterTool.setProperty("Label", m_ghostTruthTQuarksLabel));
    ASG_CHECK(m_pseudoTruthTQuarksGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthTQuarksGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthTQuarksGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthTQuarksGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthTQuarksGetterTool.getHandle());
  }
  if(!m_ghostTruthWBosonsInputContainer.empty()){
    ATH_MSG_INFO( "GhostWBosons PseudoJet Builder initializing..." );
    if(m_ghostTruthWBosonsLabel.empty()) m_ghostTruthWBosonsLabel = "Ghost" + m_ghostTruthWBosonsInputContainer;
    ASG_CHECK(m_pseudoTruthWBosonsGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthWBosonsInputContainer));
    ASG_CHECK(m_pseudoTruthWBosonsGetterTool.setProperty("OutputContainer", "GhostTruthWBosons"+name()));
    ASG_CHECK(m_pseudoTruthWBosonsGetterTool.setProperty("Label", m_ghostTruthWBosonsLabel));
    ASG_CHECK(m_pseudoTruthWBosonsGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthWBosonsGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthWBosonsGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthWBosonsGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthWBosonsGetterTool.getHandle());
  }
  if(!m_ghostTruthZBosonsInputContainer.empty()){
    ATH_MSG_INFO( "GhostZBosons PseudoJet Builder initializing..." );
    if(m_ghostTruthZBosonsLabel.empty()) m_ghostTruthZBosonsLabel = "Ghost" + m_ghostTruthZBosonsInputContainer;
    ASG_CHECK(m_pseudoTruthZBosonsGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthZBosonsInputContainer));
    ASG_CHECK(m_pseudoTruthZBosonsGetterTool.setProperty("OutputContainer", "GhostTruthZBosons"+name()));
    ASG_CHECK(m_pseudoTruthZBosonsGetterTool.setProperty("Label", m_ghostTruthZBosonsLabel));
    ASG_CHECK(m_pseudoTruthZBosonsGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthZBosonsGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthZBosonsGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthZBosonsGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthZBosonsGetterTool.getHandle());
  }
  if(!m_ghostTruthHBosonsInputContainer.empty()){
    ATH_MSG_INFO( "GhostHBosons PseudoJet Builder initializing..." );
    if(m_ghostTruthHBosonsLabel.empty()) m_ghostTruthHBosonsLabel = "Ghost" + m_ghostTruthHBosonsInputContainer;
    ASG_CHECK(m_pseudoTruthHBosonsGetterTool.setProperty("InputContainer",  "TruthLabel" + m_ghostTruthHBosonsInputContainer));
    ASG_CHECK(m_pseudoTruthHBosonsGetterTool.setProperty("OutputContainer", "GhostTruthHBosons"+name()));
    ASG_CHECK(m_pseudoTruthHBosonsGetterTool.setProperty("Label", m_ghostTruthHBosonsLabel));
    ASG_CHECK(m_pseudoTruthHBosonsGetterTool.setProperty("SkipNegativeEnergy", true));
    ASG_CHECK(m_pseudoTruthHBosonsGetterTool.setProperty("GhostScale", m_ghostScale));
    ASG_CHECK(m_pseudoTruthHBosonsGetterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_pseudoTruthHBosonsGetterTool.retrieve());
    getterArray.push_back(m_pseudoTruthHBosonsGetterTool.getHandle());
  }

  ATH_CHECK(getterArray.retrieve() );
  //    - create a Jet builder
  ATH_MSG_INFO( "Jet Builder initializing..." );
  ASG_CHECK(m_jetFromPseudoJetTool.setProperty("Attributes", areaAttributes));
  ASG_CHECK(m_jetFromPseudoJetTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_jetFromPseudoJetTool.retrieve());
  //    - create a ClusterSequence Tool
  ATH_MSG_INFO( "Cluster Sequencer initializing..." );
  ASG_CHECK(m_jetFinderTool.setProperty("JetAlgorithm", m_rc_alg));
  ASG_CHECK(m_jetFinderTool.setProperty("JetRadius", m_radius));
  ASG_CHECK(m_jetFinderTool.setProperty("VariableRMinRadius", m_varR_minR));
  ASG_CHECK(m_jetFinderTool.setProperty("VariableRMassScale", m_varR_mass*1.e3));
  ASG_CHECK(m_jetFinderTool.setProperty("PtMin", m_ptMin_rc*1.e3));
  // set ghost area, ignore if grooming is being applied to reclustered jets
  ASG_CHECK(m_jetFinderTool.setProperty("GhostArea", ghostArea));
  ASG_CHECK(m_jetFinderTool.setProperty("RandomOption", 1));
  ASG_CHECK(m_jetFinderTool.setProperty("JetBuilder", m_jetFromPseudoJetTool.getHandle()));
  ASG_CHECK(m_jetFinderTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_jetFinderTool.retrieve());
  //    - create list of modifiers.
  modArray.clear();
  //        we need to calculate effectiveR before grooming, if we are doing variableR
  ATH_CHECK(m_effectiveRTool.retrieve());
  modArray.push_back(m_effectiveRTool.getHandle() );
  ATH_CHECK(modArray.retrieve() );
  //    - create our master reclustering tool
  ATH_MSG_INFO( "Jet Reclusterer initializing..." );
  ASG_CHECK(m_reclusterJetTool.setProperty("OutputContainer", m_ungroomedJetContainer));
  ASG_CHECK(m_reclusterJetTool.setProperty("PseudoJetGetters", getterArray));
  ASG_CHECK(m_reclusterJetTool.setProperty("JetFinder", m_jetFinderTool.getHandle()));
  ASG_CHECK(m_reclusterJetTool.setProperty("JetModifiers", modArray));
  ASG_CHECK(m_reclusterJetTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_reclusterJetTool.retrieve());

  // clear modArray again
  modArray.clear();

  //        then groom the reclustered jets
  if(m_groomAlg=="trim") {
    //        using trimming
    ATH_MSG_INFO( "Jet Trimmer initializing..." );
    ATH_MSG_INFO( "\tPtFrac: " << m_trim_ptFrac );
    ATH_MSG_INFO( "\tRClus: " << m_trim_subjet_radius );
    ASG_CHECK( ASG_MAKE_ANA_TOOL( m_jetGroomingTool, JetTrimmer) );
    m_jetGroomingTool.setName("JetTrimmer/JetTrimTool_" + this->name());
    ASG_CHECK(m_jetGroomingTool.setProperty("PtFrac", m_trim_ptFrac));
    ASG_CHECK(m_jetGroomingTool.setProperty("RClus", m_trim_subjet_radius));
    ASG_CHECK(m_jetGroomingTool.setProperty("JetBuilder", m_jetFromPseudoJetTool.getHandle()));
    ASG_CHECK(m_jetGroomingTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_jetGroomingTool.retrieve() );
  }

  if(m_groomAlg=="softdrop") {
    //        using softdrop
    ATH_MSG_INFO( "Jet SoftDrop initializing..." );
    ATH_MSG_INFO( "\tZCut: " << m_sd_zcut );
    ATH_MSG_INFO( "\tBeta: " << m_sd_beta );
    ATH_MSG_INFO( "\tR0: " << m_sd_R0 );
    ATH_MSG_INFO( "\tN: " << m_sd_N );
    ASG_CHECK( ASG_MAKE_ANA_TOOL( m_jetGroomingTool, JetRecursiveSoftDrop) );
    m_jetGroomingTool.setName("JetRecursiveSoftDrop/JetRecursiveSoftDropTool_" + this->name());
    ASG_CHECK(m_jetGroomingTool.setProperty("ZCut", m_sd_zcut));
    ASG_CHECK(m_jetGroomingTool.setProperty("Beta", m_sd_beta));
    ASG_CHECK(m_jetGroomingTool.setProperty("R0", m_sd_R0));
    ASG_CHECK(m_jetGroomingTool.setProperty("JetBuilder", m_jetFromPseudoJetTool.getHandle()));
    ASG_CHECK(m_jetGroomingTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_jetGroomingTool.retrieve() );
  }

  // because of changes to JetRec, can't rely on them making this tool for us anymore
  ATH_MSG_INFO( "JetPseudojetRetriever initializing for Jet Groomer..." );
  ASG_CHECK(m_jetGroomTool_JPJR.retrieve());

  //        and then apply all other modifiers based on the groomed reclustered jets
  ATH_MSG_INFO( "\t... and queuing up various jet modifiers..." );

  ASG_CHECK(m_jetChargeTool.retrieve());
  modArray.push_back(m_jetChargeTool.getHandle() );

  ASG_CHECK(m_jetPullTool.retrieve());
  modArray.push_back(m_jetPullTool.getHandle() );

  ASG_CHECK(m_energyCorrelatorTool.setProperty("BetaList", m_ecf_betaVals));
  ASG_CHECK(m_energyCorrelatorTool.setProperty("DoDichroic", m_ecf_doDichroic));
  ASG_CHECK(m_energyCorrelatorTool.setProperty("DoC3", m_ecf_doC3));
  ASG_CHECK(m_energyCorrelatorTool.retrieve());
  modArray.push_back(m_energyCorrelatorTool.getHandle() );

  ASG_CHECK(m_energyCorrelatorRatiosTool.setProperty("BetaList", m_ecf_betaVals));
  ASG_CHECK(m_energyCorrelatorRatiosTool.setProperty("DoDichroic", m_ecf_doDichroic));
  ASG_CHECK(m_energyCorrelatorRatiosTool.setProperty("DoC3", m_ecf_doC3));
  ASG_CHECK(m_energyCorrelatorRatiosTool.retrieve());
  modArray.push_back(m_energyCorrelatorRatiosTool.getHandle() );

  ASG_CHECK(m_energyCorrelatorGeneralizedTool.setProperty("BetaList", m_ecf_betaVals));
  ASG_CHECK(m_energyCorrelatorGeneralizedTool.setProperty("DoDichroic", m_ecf_doDichroic));
  ASG_CHECK(m_energyCorrelatorGeneralizedTool.setProperty("DoN3", m_ecfg_doN3));
  ASG_CHECK(m_energyCorrelatorGeneralizedTool.setProperty("DoLSeries", m_ecfg_doLSeries));
  ASG_CHECK(m_energyCorrelatorGeneralizedTool.retrieve());
  modArray.push_back(m_energyCorrelatorGeneralizedTool.getHandle() );

  ASG_CHECK(m_energyCorrelatorGeneralizedRatiosTool.setProperty("BetaList", m_ecf_betaVals));
  ASG_CHECK(m_energyCorrelatorGeneralizedRatiosTool.setProperty("DoDichroic", m_ecf_doDichroic));
  ASG_CHECK(m_energyCorrelatorGeneralizedRatiosTool.retrieve());
  modArray.push_back(m_energyCorrelatorGeneralizedRatiosTool.getHandle() );

  ASG_CHECK(m_ktSplittingScaleTool.retrieve());
  modArray.push_back(m_ktSplittingScaleTool.getHandle() );

  ASG_CHECK(m_dipolarityTool.retrieve());
  modArray.push_back(m_dipolarityTool.getHandle() );

  ASG_CHECK(m_centerOfMassShapesTool.retrieve());
  modArray.push_back(m_centerOfMassShapesTool.getHandle() );

  ASG_CHECK(m_nSubjettinessTool.retrieve());
  modArray.push_back(m_nSubjettinessTool.getHandle() );
  ATH_CHECK(modArray.retrieve() );
  // finish up the rest of the tool

  ASG_CHECK(m_groomJetTool.setProperty("InputContainer", m_ungroomedJetContainer));
  ASG_CHECK(m_groomJetTool.setProperty("OutputContainer", m_outputJetContainer));
  ASG_CHECK(m_groomJetTool.setProperty("JetModifiers", modArray));
  ASG_CHECK(m_groomJetTool.setProperty("JetGroomer",  m_jetGroomingTool.getHandle() ));
  ASG_CHECK(m_groomJetTool.setProperty("JetPseudojetRetriever", m_jetGroomTool_JPJR.getHandle()));
  ASG_CHECK(m_groomJetTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_groomJetTool.retrieve());

  return StatusCode::SUCCESS;
}

int JetReclusteringTool::execute() const {
  int retCode = 0;
  if(m_ptMin_input > 0) {
    if ( (retCode = m_inputJetFilterTool->execute() ) != 0) {
      ATH_MSG_ERROR( "Failure in inputJetFilterTool with return code: " << retCode );
      return retCode;
    }
  }
  if ( (retCode = m_reclusterJetTool->execute() ) != 0) {
    ATH_MSG_ERROR( "Failure in reclusterJetTool with return code: " << retCode );
    return retCode;
  }
  if ( (retCode = m_groomJetTool->execute() ) != 0) {
    ATH_MSG_ERROR( "Failure in groomJetTool with return code: " << retCode );
    return retCode;
  }
  return 0;
}

void JetReclusteringTool::print() const {
  ATH_MSG_INFO("Properties for " << m_APP_NAME << "(" << name() << ")");
  ATH_MSG_INFO("    InputJetContainer:      " << m_inputJetContainer );
  ATH_MSG_INFO("    OutputJetContainer:     " << m_outputJetContainer );
  ATH_MSG_INFO("    Radius:                 " << m_radius );
  ATH_MSG_INFO("    ReclusteringAlgorithm:  " << m_rc_alg << " (" << m_rc_alg << ")" );
  ATH_MSG_INFO("    VariableRMinRadius:     " << m_varR_minR );
  ATH_MSG_INFO("    VariableRMassScale:     " << m_varR_mass << " GeV" );
  ATH_MSG_INFO("    InputJetPtCut:          " << m_ptMin_input << " GeV" );
  ATH_MSG_INFO("    ReclusteredJetPtCut:    " << m_ptMin_rc << " GeV" );
  if(m_groomAlg=="trim") {
    ATH_MSG_INFO("    TrimPtFrac:             " << m_trim_ptFrac );
    ATH_MSG_INFO("    TrimJetSubjetR:         " << m_trim_subjet_radius );
  }
  if(m_groomAlg=="softdrop") {
    ATH_MSG_INFO("    SoftDropZCut:           " << m_sd_zcut );
    ATH_MSG_INFO("    SoftDropBeta:           " << m_sd_beta );
    ATH_MSG_INFO("    SoftDropR0:             " << m_sd_R0 );
    ATH_MSG_INFO("    SoftDropN:              " << m_sd_N );
  }

  if(m_isInitialized){
    m_inputJetFilterTool->print();
    m_reclusterJetTool->print();
    m_groomJetTool->print();
  } else {
    ATH_MSG_INFO(m_APP_NAME << " has not been initialized yet");
  }

  return;
}
