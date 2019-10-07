/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPPARTONS_CALCThqPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCThqARTONHISTORY_H


// Framework include(s):
#include "TopPartons/CalcTopPartonHistory.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class CalcThqPartonHistory : public CalcTopPartonHistory{
    public:
      explicit CalcThqPartonHistory( const std::string& name );
      virtual ~CalcThqPartonHistory() {}

      struct tH_values{
        //Higgs
        TLorentzVector Higgs_p4;
        TLorentzVector Tau1_from_Higgs_p4;
        int Tau1_from_Higgs_pdgId;
        TLorentzVector Tau2_from_Higgs_p4;
        int Tau2_from_Higgs_pdgId;
        TLorentzVector nu_from_Tau1_p4;
        int nu_from_Tau1_pdgId;
        TLorentzVector nu_from_Tau2_p4;
        int nu_from_Tau2_pdgId;
        TLorentzVector W_decay1_from_Tau1_p4;
        int W_decay1_from_Tau1_pdgId;
        TLorentzVector W_decay2_from_Tau1_p4;
        int W_decay2_from_Tau1_pdgId;
        TLorentzVector W_decay1_from_Tau2_p4;
        int W_decay1_from_Tau2_pdgId;
        TLorentzVector W_decay2_from_Tau2_p4;
        int W_decay2_from_Tau2_pdgId;

        //Bools
        int TauJets1; 
        int TauJets2; 
    
        //b
        TLorentzVector b_p4;
        int b_pdgId;

      }tH;
      //Storing parton history for ttbar resonance analysis
      CalcThqPartonHistory(const CalcThqPartonHistory& rhs) = delete;
      CalcThqPartonHistory(CalcThqPartonHistory&& rhs) = delete;
      CalcThqPartonHistory& operator=(const CalcThqPartonHistory& rhs) = delete;
      
    void THHistorySaver(const xAOD::TruthParticleContainer * truthParticles, xAOD::PartonHistory * ThqPartonHistory);
    
    //handle gamma radiation of taus
    const xAOD::TruthParticle* findAfterGamma(const xAOD::TruthParticle* particle);

    ///Store the four-momentum of several particles in the Higgs decay chain
    bool Higgstautau( const xAOD::TruthParticleContainer* truthParticles, int start);

    //Store four-momentum of bottom quark 
    bool bottom(const xAOD::TruthParticleContainer* truthParticles, int start);
   
    int sign(int a);

    virtual StatusCode execute();

  };
  
}

#endif
