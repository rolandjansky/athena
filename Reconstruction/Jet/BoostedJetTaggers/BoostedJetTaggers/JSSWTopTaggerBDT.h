/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JSSWTOPTAGGERBDT_H_
#define JSSWTOPTAGGERBDT_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

class JSSWTopTaggerBDT :
  public JSSTaggerBase {
    ASG_TOOL_CLASS0(JSSWTopTaggerBDT)

    public:

      /// Constructor
      JSSWTopTaggerBDT( const std::string& name );

      /// Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      /// IJetSelectorTool interface
      virtual Root::TAccept& tag( const xAOD::Jet& jet ) const override;

    private:

      /// TMVA tools
      std::unique_ptr<TMVA::Reader> m_bdtTagger;
      std::map<std::string,std::string> m_bdt_weights;
      std::string m_inputVariableSet;
      std::string m_BDTmethod;

      /// Variables for TMVA
      mutable float m_mass; // combined mass
      mutable float m_pt;   // calibrated pT, corrected by combined mass
      mutable float m_e3;   // ECF3/ECF1**3
      mutable float m_D2;
      mutable float m_C2;
      mutable float m_Tau1_wta;
      mutable float m_Tau2_wta;
      mutable float m_Tau21_wta;
      mutable float m_Tau32_wta;
      mutable float m_Split12;
      mutable float m_Split23;
      mutable float m_Qw;
      mutable float m_KtDR;
      mutable float m_Aplanarity;
      mutable float m_Angularity;
      mutable float m_PlanarFlow;
      mutable float m_FoxWolfram20;

      /// Retrieve BDT score
      float getScore( const xAOD::Jet& jet ) const;

      /// Update the jet substructure variables for each jet to use in BDT
      void getJetProperties( const xAOD::Jet& jet ) const;

  };

#endif
