/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthJetDecorationTool.cxx
// Author: Chris Young (christopher.young@cern.ch)
// Create decorations on truth jets upon detector response might depend...

#include "DerivationFrameworkMCTruth/TruthJetDecorationTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include <string>

// Constructor
DerivationFramework::TruthJetDecorationTool::TruthJetDecorationTool(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty ("JetCollection",
          m_jetsKey = "AntiKt4TruthJets",
          "Name of jet collection for decoration");
}

// Destructor
DerivationFramework::TruthJetDecorationTool::~TruthJetDecorationTool() {
}

// Function to do dressing, implements interface in IAugmentationTool
StatusCode DerivationFramework::TruthJetDecorationTool::addBranches() const
{
  // Retrieve the truth collections
  const DataHandle<xAOD::JetContainer> inputJets(nullptr);
  CHECK(evtStore()->retrieve(inputJets, m_jetsKey));

  const static SG::AuxElement::Decorator<float> output_decorator_emfrac("DFTruthJet_EMFrac");
  const static SG::AuxElement::Decorator<float> output_decorator_500MeVfrac("DFTruthJet_lt500MeVFrac");
  const static SG::AuxElement::Decorator<float> output_decorator_1000MeVfrac("DFTruthJet_lt1000MeVFrac");
  const static SG::AuxElement::Decorator<float> output_decorator_500MeVfracem("DFTruthJet_lt500MeVFracEM");
  const static SG::AuxElement::Decorator<float> output_decorator_1000MeVfracem("DFTruthJet_lt1000MeVFracEM");
  const static SG::AuxElement::Decorator<float> output_decorator_bfrac("DFTruthJet_BaryonFrac");
  const static SG::AuxElement::Decorator<float> output_decorator_klfrac("DFTruthJet_KLFrac");
  const static SG::AuxElement::Decorator<float> output_decorator_ksfrac("DFTruthJet_KSFrac");
  const static SG::AuxElement::Decorator<float> output_decorator_kfrac("DFTruthJet_KaonFrac");
  const static SG::AuxElement::Decorator<int> output_decorator_numb("DFTruthJet_numBaryon");
  const static SG::AuxElement::Decorator<int> output_decorator_numkl("DFTruthJet_numKL");
  const static SG::AuxElement::Decorator<int> output_decorator_numks("DFTruthJet_numKS");
  const static SG::AuxElement::Decorator<int> output_decorator_numk("DFTruthJet_numKaon");

  for (auto ajet : *inputJets){
    float sumE=0.0;
    float EM_sumE=0.0;
    float lt500MeV_sumE=0.0;
    float lt500MeVEM_sumE=0.0;
    float lt1000MeV_sumE=0.0;
    float lt1000MeVEM_sumE=0.0;
    float baryon_sumE=0.0;
    float KL_sumE=0.0;
    float KS_sumE=0.0;
    float K_sumE=0.0;
    int numBaryon=0;
    int numKL=0;
    int numKS=0;
    int numK=0;
    for (auto apart : ajet->constituentLinks() ) {
      const xAOD::TruthParticle *part = static_cast<const xAOD::TruthParticle*>(*apart);
      if( ! (part->status() == 1) ) continue;
      if ((part->barcode())>2e5) continue;
      sumE += part->e();
      if (part->pdgId()==22 || abs(part->pdgId())==11) EM_sumE += part->e();
      if (part->pt()<500.0) lt500MeV_sumE += part->e();
      if (part->pt()<1000.0) lt1000MeV_sumE += part->e();
      if (part->pt()<500.0 && (part->pdgId()==22 || abs(part->pdgId())==11)) lt500MeVEM_sumE += part->e();
      if (part->pt()<1000.0 && (part->pdgId()==22 || abs(part->pdgId())==11)) lt1000MeVEM_sumE += part->e();
      if (abs(part->pdgId())>=1000 && abs(part->pdgId())<=9999) baryon_sumE += part->e();
      if (abs(part->pdgId())>=1000 && abs(part->pdgId())<=9999) numBaryon++;
      if (abs(part->pdgId())==130) KL_sumE += part->e();
      if (abs(part->pdgId())==130) numKL++;
      if (abs(part->pdgId())==310) KS_sumE += part->e();
      if (abs(part->pdgId())==310) numKS++;
      if (abs(part->pdgId())==321) K_sumE += part->e();
      if (abs(part->pdgId())==321) numK++;
    } // Loop over constituents
    output_decorator_emfrac(*ajet) = EM_sumE/sumE;
    output_decorator_500MeVfrac(*ajet) = lt500MeV_sumE/sumE;
    output_decorator_1000MeVfrac(*ajet) = lt1000MeV_sumE/sumE;
    output_decorator_500MeVfracem(*ajet) = lt500MeVEM_sumE/sumE;
    output_decorator_1000MeVfracem(*ajet) = lt1000MeVEM_sumE/sumE;
    output_decorator_bfrac(*ajet) = baryon_sumE/sumE;
    output_decorator_klfrac(*ajet) = KL_sumE/sumE;
    output_decorator_ksfrac(*ajet) = KS_sumE/sumE;
    output_decorator_kfrac(*ajet) = K_sumE/sumE;
    output_decorator_numb(*ajet) = numBaryon;
    output_decorator_numkl(*ajet) = numKL;
    output_decorator_numks(*ajet) = numKS;
    output_decorator_numk(*ajet) = numK;

  } // Loop over jets

  return StatusCode::SUCCESS;
}
