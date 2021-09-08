/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include <IsolationSelection/IsolationLowPtPLVTool.h>
#include <xAODBase/ObjectType.h>
#include <xAODBase/IParticleHelpers.h>

//Tools includes:
#include <cmath>
#include "PathResolver/PathResolver.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"

namespace CP {
  // Accessors for input variables
  static const ShortAccessor s_acc_TrackJetNTrack("PromptLeptonInput_TrackJetNTrack");
  static const FloatAccessor s_acc_DRlj("PromptLeptonInput_DRlj");
  static const FloatAccessor s_acc_PtRel("PromptLeptonInput_PtRel");
  static const FloatAccessor s_acc_PtFrac("PromptLeptonInput_PtFrac");
  static const FloatAccessor s_acc_topoetcone20("topoetcone20");
  static const FloatAccessor s_acc_ptvarcone20("ptvarcone20");
  static const FloatAccessor s_acc_ptvarcone30("ptvarcone30");
  static const FloatAccessor s_acc_PLV("PromptLeptonVeto");
  static const FloatDecorator s_dec_iso_PLT("LowPtPLV");    
  
  IsolationLowPtPLVTool::IsolationLowPtPLVTool(const std::string& toolName) :
    asg::AsgTool(toolName){
    // Decorator for scores
    declareProperty( "MuonCalibFile", m_muonCalibFile = "IsolationCorrections/v5/TMVAClassification_BDT_Muon_LowPtPromptLeptonTagger_20191107.weights.xml", 
		     "XML file holding the TMVA configuration for muons" );
    declareProperty( "ElecCalibFile", m_elecCalibFile = "IsolationCorrections/v5/TMVAClassification_BDT_Electron_LowPtPromptLeptonTagger_20191107.weights.xml", 
		     "XML file holding the TMVA configuration for electrons");
    declareProperty( "MuonMethodName", m_muonMethodName = "LowPtPLT_Muon", "Method name for electron LowPtPLV" );
    declareProperty( "ElecMethodName", m_elecMethodName = "LowPtPLT_Elec", "Method name for muon LowPtPLV" );
  }
  
  StatusCode IsolationLowPtPLVTool::initialize() {
    TMVA::Tools::Instance();
    m_TMVAReader_Muon = std::make_unique<TMVA::Reader>("!Silent:Color");
    m_TMVAReader_Elec = std::make_unique<TMVA::Reader>("!Silent:Color");

    std::string fullPathToFile_Muon = PathResolverFindCalibFile(m_muonCalibFile);
    std::string fullPathToFile_Elec = PathResolverFindCalibFile(m_elecCalibFile);

    static const std::array< std::string, N_VARIABLES > BDT_vars_Muon {
      "TrackJetNTrack",
      "PtRel",
      "PtFrac",
      "DRlj",
      "TopoEtCone20Rel",
      "PtVarCone30Rel"
	};

    static const std::array< std::string, N_VARIABLES > BDT_vars_Elec {
      "TrackJetNTrack",
      "PtRel",
      "PtFrac",
      "DRlj",
      "TopoEtCone20Rel",
      "PtVarCone20Rel"
    };

    for (int i=0; i<N_VARIABLES; i++){
      m_TMVAReader_Muon->AddVariable(BDT_vars_Muon[i], &m_varTMVA_Muon[i]);
    }

    for (int i=0; i<N_VARIABLES; i++){
      m_TMVAReader_Elec->AddVariable(BDT_vars_Elec[i], &m_varTMVA_Elec[i]);
    }

    if (fullPathToFile_Muon == ""){
      ATH_MSG_ERROR("Error! No xml file found for Muon LowPtPLV");
      return StatusCode::FAILURE;
    }
    m_TMVAReader_Muon->BookMVA(m_muonMethodName, fullPathToFile_Muon);
    TMVA::MethodBDT* method_Muon_bdt = dynamic_cast<TMVA::MethodBDT*> (m_TMVAReader_Muon->FindMVA(m_muonMethodName));
    if(!method_Muon_bdt){    
      ATH_MSG_ERROR("Error! No method found for Muon LowPtPLV");
      return StatusCode::FAILURE;
    }

    if (fullPathToFile_Elec == ""){
      ATH_MSG_ERROR("Error! No xml file found for Electron LowPtPLV");
      return StatusCode::FAILURE;
    }    m_TMVAReader_Elec->BookMVA(m_elecMethodName, fullPathToFile_Elec);
    TMVA::MethodBDT* method_Elec_bdt = dynamic_cast<TMVA::MethodBDT*> (m_TMVAReader_Elec->FindMVA(m_elecMethodName));
    if(!method_Elec_bdt){    
      ATH_MSG_ERROR("Error! No method found for Electron LowPtPLV");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_INFO("Initialized IsolationLowPtPLVTool");
    return StatusCode::SUCCESS;
  }
    
  StatusCode IsolationLowPtPLVTool::augmentPLV( const xAOD::IParticle& Particle) {

    float pt = Particle.pt();

    //Check is PLV variable is set
    if( s_acc_PLV.isAvailable(Particle) ) {
      if( s_acc_PLV(Particle) <= -1.1 ){
	ATH_MSG_DEBUG( "Nominal PLV score " << s_acc_PLV(Particle) << ". Returning -1.1" );
	s_dec_iso_PLT(Particle) = -1.1;
	return StatusCode::SUCCESS;
      }
    }

    // Check if input variables exist (warning messages only for low-pt leptons)
    bool varMissingMsg = pt < 12e3 && varMissingMsgCounter <= N_MSG_COUNTS;

    bool inputvar_missing = false;
    if (!s_acc_TrackJetNTrack.isAvailable(Particle)){
      if(varMissingMsg) ATH_MSG_WARNING( "Variable \"TrackJetNTrack\" not available" );
      inputvar_missing = true;
    }

    if (!s_acc_DRlj.isAvailable(Particle)){
      if(varMissingMsg) ATH_MSG_WARNING( "Variable \"DRlj\" not available" );
      inputvar_missing = true;
    }

    if (!s_acc_PtRel.isAvailable(Particle)){
      if(varMissingMsg) ATH_MSG_WARNING( "Variable \"PtRel\" not available" );
      inputvar_missing = true;
    }

    if (!s_acc_PtFrac.isAvailable(Particle)){
      if(varMissingMsg) ATH_MSG_WARNING( "Variable \"PtFrac\" not available" );
      inputvar_missing = true;
    }

    if (!s_acc_topoetcone20.isAvailable(Particle)){
      if(varMissingMsg) ATH_MSG_WARNING( "Variable \"topoetcone20\" not available" );
      inputvar_missing = true;
    }

    if (Particle.type() == xAOD::Type::ObjectType::Electron && !s_acc_ptvarcone20.isAvailable(Particle)){
      if(varMissingMsg) ATH_MSG_WARNING( "Variable \"ptvarcone20\" not available" );
      inputvar_missing = true;
    }

    if (Particle.type() == xAOD::Type::ObjectType::Muon && !s_acc_ptvarcone30.isAvailable(Particle)){
      if(varMissingMsg) ATH_MSG_WARNING( "Variable \"ptvarcone30\" not available" );
      inputvar_missing = true;
    }

    if (inputvar_missing){
      if(varMissingMsg){
	ATH_MSG_WARNING( "INPUT VARIABLE(S) FOR LOW PT LEPTON (" << pt << ") MISSING. AUGMENTING FIXED VALUE 1.1");
	if(varMissingMsgCounter==N_MSG_COUNTS) ATH_MSG_WARNING( "REACHED "<< varMissingMsgCounter << " WARNING MESSAGES FOR MISSING VARIABLES... SILENCING");
	varMissingMsgCounter++;
      }
      s_dec_iso_PLT(Particle) = 1.1;
      return StatusCode::SUCCESS;
    }

    short TrackJetNTrack = s_acc_TrackJetNTrack(Particle);
    float DRlj         = s_acc_DRlj(Particle);
    float PtRel        = s_acc_PtRel(Particle);
    float PtFrac       = s_acc_PtFrac(Particle);
    float topoetcone20 = s_acc_topoetcone20(Particle);
    float ptvarcone30  = 0;
    float ptvarcone20  = 0;
    float score        = 1.1;

    if (Particle.type() == xAOD::Type::ObjectType::Muon){
      ptvarcone30  = s_acc_ptvarcone30(Particle);
      m_varTMVA_Muon[0] = TrackJetNTrack;
      m_varTMVA_Muon[1] = PtRel;
      m_varTMVA_Muon[2] = PtFrac;
      m_varTMVA_Muon[3] = DRlj;
      m_varTMVA_Muon[4] = topoetcone20/pt;
      m_varTMVA_Muon[5] = ptvarcone30/pt;
      score = m_TMVAReader_Muon->EvaluateMVA(m_muonMethodName);
    }
    else if (Particle.type() == xAOD::Type::ObjectType::Electron){
      ptvarcone20  = s_acc_ptvarcone20(Particle);
      m_varTMVA_Elec[0] = TrackJetNTrack;
      m_varTMVA_Elec[1] = PtRel;
      m_varTMVA_Elec[2] = PtFrac;
      m_varTMVA_Elec[3] = DRlj;
      m_varTMVA_Elec[4] = topoetcone20/pt;
      m_varTMVA_Elec[5] = ptvarcone20/pt;
      score =  m_TMVAReader_Elec->EvaluateMVA(m_elecMethodName);
    }
    else {
      ATH_MSG_ERROR( "The function needs either a muon or an electron!" );
      return StatusCode::FAILURE;
    }
    s_dec_iso_PLT(Particle) = score;

    return StatusCode::SUCCESS;
  }
}
