// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JSSWTOPTAGGERBDT_H_
#define JSSWTOPTAGGERBDT_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include <TSystem.h>
#include <TFile.h>
#include <TF1.h>

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <set>
#include <list>
#include <vector>


class JSSWTopTaggerBDT : public JSSTaggerBase {
  ASG_TOOL_CLASS0(JSSWTopTaggerBDT)

  public:

    //Default - so root can load based on a name
    JSSWTopTaggerBDT(const std::string& name);

    // Default - so we can clean up
    ~JSSWTopTaggerBDT();

    JSSWTopTaggerBDT& operator=(const JSSWTopTaggerBDT& rhs);

    // Run once at the start of the job to setup everything
    StatusCode initialize();

    // IJetSelectorTool interface
    virtual Root::TAccept tag(const xAOD::Jet& jet) const;

    // Retrieve BDT score
    float getScore(const xAOD::Jet& jet) const;

    // Write the decoration to the jet
    void decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue) const;

    // Update the jet substructure variables for each jet to use in BDT
    void getJetProperties(const xAOD::Jet& jet) const;

    StatusCode finalize();

  private:

    // naming of tool
    std::string m_name;
    std::string m_APP_NAME;

    // TMVA tools
    std::unique_ptr<TMVA::Reader> m_bdtTagger;
    std::map<std::string,std::string> m_bdt_weights;
    std::string m_inputVariableSet;
    std::string m_BDTmethod;

    // inclusive config file
    std::string m_configFile;
    std::string m_tagType;
    std::string m_tmvaConfigFileName;
    std::string m_tmvaConfigFilePath;

    // variables for TMVA
    mutable float m_mass; // combinedmass
    mutable float m_pt;   // calibrated pT, corrected by combined mass
    mutable float m_ECF1;
    mutable float m_ECF2;
    mutable float m_ECF3;
    mutable float m_e3;   // ECF3/ECF1**3
    mutable float m_D2;
    mutable float m_C2;
    mutable float m_Tau1_wta;
    mutable float m_Tau2_wta;
    mutable float m_Tau3_wta;
    mutable float m_Tau21_wta;
    mutable float m_Tau32_wta;
    mutable float m_Split12;
    mutable float m_Split23;
    mutable float m_Qw;
    mutable float m_KtDR;
    mutable float m_Dip12;
    mutable float m_ZCut12;
    mutable float m_Aplanarity;
    mutable float m_Angularity;
    mutable float m_PlanarFlow;
    mutable float m_Sphericity;
    mutable float m_ThrustMin;
    mutable float m_ThrustMaj;
    mutable float m_FoxWolfram20;

    // bool to check whether variables are undefined
    mutable  bool m_undefInput;

    // parameters to store specific cut values
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strScoreCut;

    // functions that are configurable for specific cut values
    TF1* m_funcMassCutLow;
    TF1* m_funcMassCutHigh;
    TF1* m_funcScoreCut;

    // string for decorating jets with DNN output
    std::string m_decorationName;

    // decorators
    SG::AuxElement::Decorator<float> m_dec_mcutL;
    SG::AuxElement::Decorator<float> m_dec_mcutH;
    SG::AuxElement::Decorator<float> m_dec_scoreCut;
    SG::AuxElement::Decorator<float> m_dec_scoreValue;

};

#endif
