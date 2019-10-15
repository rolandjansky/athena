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
  static const IntAccessor m_acc_TrackJetNTrack("PromptLeptonInput_TrackJetNTrack");
  static const FloatAccessor m_acc_DRlj("PromptLeptonInput_DRlj");
  static const FloatAccessor m_acc_PtRel("PromptLeptonInput_PtRel");
  static const FloatAccessor m_acc_PtFrac("PromptLeptonInput_PtFrac");
  static const FloatAccessor m_acc_topoetcone20("topoetcone20");
  static const FloatAccessor m_acc_ptvarcone20("ptvarcone20");
  static const FloatAccessor m_acc_ptvarcone30("ptvarcone30");
  static const FloatDecorator m_dec_iso_PLT("LowPtPLV");    
  
  IsolationLowPtPLVTool::IsolationLowPtPLVTool(const std::string& toolName) :
    asg::AsgTool(toolName){
    // Decorator for scores
    declareProperty( "MuonCalibFile", m_muonCalibFile, "XML file holding the TMVA configuration for muons" );
    declareProperty( "ElecCalibFile", m_elecCalibFile, "XML file holding the TMVA configuration for electrons");
  }
  
  StatusCode IsolationLowPtPLVTool::initialize() {
    TMVA::Tools::Instance();
    m_TMVAReader_Muon = std::make_unique<TMVA::Reader>("!Silent:Color");
    m_TMVAReader_Elec = std::make_unique<TMVA::Reader>("!Silent:Color");

    std::string fullPathToFile_Muon = PathResolverFindCalibFile(m_muonCalibFile);
    std::string fullPathToFile_Elec = PathResolverFindCalibFile(m_elecCalibFile);
    
    m_muonMethodName = "LowPtPLT_Muon";
    m_elecMethodName = "LowPtPLT_Elec";

    static const std::array< std::string, N_VARIABLES > BDT_vars_Muon {
      "CorrPtvarcone30/Pt/1000",
      "CorrTopoetcone20/Pt",
      "PLTInput_PtRel",
      "PLTInput_PtFrac",
      "PLTInput_DRlj",
      "PLTInput_TrackJetNTrack"
    };

    static const std::array< std::string, N_VARIABLES > BDT_vars_Elec {
      "CorrPtvarcone20/Pt/1000",
      "CorrTopoetcone20/Pt",
      "PLTInput_PtRel",
      "PLTInput_PtFrac",
      "PLTInput_DRlj",
      "PLTInput_TrackJetNTrack"
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
    if (!m_acc_TrackJetNTrack.isAvailable(Particle)){
      ATH_MSG_ERROR( "TrackJetNTrack not available" );
      return StatusCode::FAILURE;
    }

    if (!m_acc_DRlj.isAvailable(Particle)){
      ATH_MSG_ERROR( "DRlj not available" );
      return StatusCode::FAILURE;
    }

    if (!m_acc_PtRel.isAvailable(Particle)){
      ATH_MSG_ERROR( "PtRel not available" );
      return StatusCode::FAILURE;
    }

    if (!m_acc_PtFrac.isAvailable(Particle)){
      ATH_MSG_ERROR( "PtFrac not available" );
      return StatusCode::FAILURE;
    }

    if (!m_acc_topoetcone20.isAvailable(Particle)){
      ATH_MSG_ERROR( "topoetcone20 not available" );
      return StatusCode::FAILURE;
    }

    if (!m_acc_ptvarcone20.isAvailable(Particle)){
      ATH_MSG_ERROR( "ptvarcone20 not available" );
      return StatusCode::FAILURE;
    }

    if (!m_acc_ptvarcone30.isAvailable(Particle)){
      ATH_MSG_ERROR( "ptvarcone30 not available" );
      return StatusCode::FAILURE;
    }

    
    int TrackJetNTrack = m_acc_TrackJetNTrack(Particle);
    float DRlj         = m_acc_DRlj(Particle);
    float PtRel        = m_acc_PtRel(Particle);
    float PtFrac       = m_acc_PtFrac(Particle);
    float topoetcone20 = m_acc_topoetcone20(Particle);
    float ptvarcone20  = m_acc_ptvarcone20(Particle);
    float ptvarcone30  = m_acc_ptvarcone30(Particle);
    float pt = Particle.pt();

    m_varTMVA_Muon[0] = topoetcone20/pt;
    m_varTMVA_Muon[1] = ptvarcone30/pt;
    m_varTMVA_Muon[2] = PtRel;
    m_varTMVA_Muon[3] = PtFrac;
    m_varTMVA_Muon[4] = DRlj;
    m_varTMVA_Muon[5] = TrackJetNTrack;

    m_varTMVA_Elec[0] = topoetcone20/pt;
    m_varTMVA_Elec[1] = ptvarcone20/pt;
    m_varTMVA_Elec[2] = PtRel;
    m_varTMVA_Elec[3] = PtFrac;
    m_varTMVA_Elec[4] = DRlj;
    m_varTMVA_Elec[5] = TrackJetNTrack;

    float score = 1.1;
    if (Particle.type() == xAOD::Type::ObjectType::Muon) score =  m_TMVAReader_Muon->EvaluateMVA(m_muonMethodName);
    else if (Particle.type() == xAOD::Type::ObjectType::Electron) score =  m_TMVAReader_Elec->EvaluateMVA(m_elecMethodName);
    else return StatusCode::FAILURE;
    m_dec_iso_PLT(Particle) = score;

    return StatusCode::SUCCESS;
  }
}
