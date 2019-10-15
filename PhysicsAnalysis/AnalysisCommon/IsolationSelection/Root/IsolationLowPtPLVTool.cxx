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

namespace CP {
  IsolationLowPtPLVTool::IsolationLowPtPLVTool(const std::string& toolName) :
    asg::AsgTool(toolName),

    // Accessors for input variables
    m_acc_TrackJetNTrack("PromptLeptonInput_TrackJetNTrack"),
    m_acc_DRlj("PromptLeptonInput_DRlj"),
    m_acc_PtRel("PromptLeptonInput_PtRel"),
    m_acc_PtFrac("PromptLeptonInput_PtFrac"),
    m_acc_topoetcone20("topoetcone20"),
    m_acc_ptvarcone20("ptvarcone20"),
    m_acc_ptvarcone30("ptvarcone30"),
    
    // Decorator for scores
    m_dec_iso_PLT("LowPtPLV"){
  }
  
  StatusCode IsolationLowPtPLVTool::initialize() {
    TMVA::Tools::Instance();
    m_TMVAReader_Muon = std::make_unique<TMVA::Reader>("!Silent:Color");
    m_TMVAReader_Elec = std::make_unique<TMVA::Reader>("!Silent:Color");

    std::string fullPathToFile_Muon = PathResolverFindCalibFile("IsolationCorrections/v4/TMVAClassification_BDT_Muon_LowPtPromptLeptonTagger_20181001.weights.xml");
    std::string fullPathToFile_Elec = PathResolverFindCalibFile("IsolationCorrections/v4/TMVAClassification_BDT_Electron_LowPtPromptLeptonTagger_20181001.weights.xml");
    
    std::string BDT_vars_Muon[6];
    std::string BDT_vars_Elec[6];

    BDT_vars_Muon[0] = "CorrPtvarcone30/Pt/1000";
    BDT_vars_Muon[1] = "CorrTopoetcone20/Pt";
    BDT_vars_Muon[2] = "PLTInput_PtRel";
    BDT_vars_Muon[3] = "PLTInput_PtFrac";
    BDT_vars_Muon[4] = "PLTInput_DRlj";
    BDT_vars_Muon[5] = "PLTInput_TrackJetNTrack";

    BDT_vars_Elec[0] = "CorrPtvarcone20/Pt/1000";
    BDT_vars_Elec[1] = "CorrTopoetcone20/Pt";
    BDT_vars_Elec[2] = "PLTInput_PtRel";
    BDT_vars_Elec[3] = "PLTInput_PtFrac";
    BDT_vars_Elec[4] = "PLTInput_DRlj";
    BDT_vars_Elec[5] = "PLTInput_TrackJetNTrack";

    m_varTMVA_Muon = std::make_unique<float []>(6);
    m_varTMVA_Elec = std::make_unique<float []>(6);

    for (int i=0; i<6; i++){
      m_TMVAReader_Muon->AddVariable(BDT_vars_Muon[i], &m_varTMVA_Muon[i]);
    }

    for (int i=0; i<6; i++){
      m_TMVAReader_Elec->AddVariable(BDT_vars_Elec[i], &m_varTMVA_Elec[i]);
    }
    
    m_TMVAReader_Muon->BookMVA("LowPtPLT_Muon", fullPathToFile_Muon);
    m_TMVAReader_Elec->BookMVA("LowPtPLT_Elec", fullPathToFile_Elec);
    
    ATH_MSG_INFO("Initialized IsolationLowPtPLVTool");
    return StatusCode::SUCCESS;
  }
    
  StatusCode IsolationLowPtPLVTool::AugmentPLV(xAOD::IParticle* Particle) const {
    int TrackJetNTrack = m_acc_TrackJetNTrack(*Particle);
    float DRlj         = m_acc_DRlj(*Particle);
    float PtRel        = m_acc_PtRel(*Particle);
    float PtFrac       = m_acc_PtFrac(*Particle);
    float topoetcone20 = m_acc_topoetcone20(*Particle);
    float ptvarcone20  = m_acc_ptvarcone20(*Particle);
    float ptvarcone30  = m_acc_ptvarcone30(*Particle);
    float pt = Particle->pt();

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
    if (Particle->type() == xAOD::Type::ObjectType::Muon) score =  m_TMVAReader_Muon->EvaluateMVA("LowPtPLT_Muon");
    else if (Particle->type() == xAOD::Type::ObjectType::Electron) score =  m_TMVAReader_Elec->EvaluateMVA("LowPtPLT_Elec");
    m_dec_iso_PLT(*Particle) = score;

    return StatusCode::SUCCESS;
  }
}
