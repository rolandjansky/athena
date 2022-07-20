/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "BoostedJetTaggers/SmoothedWZDisCoJetTagger.h"

class SmoothedWZTagger :
  public JSSTaggerBase {
    ASG_TOOL_CLASS0(SmoothedWZTagger)
  
    public:

      /// Constructor
      SmoothedWZTagger( const std::string& name );

      /// Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      /// IJetSelectorTool interface
      virtual Root::TAccept& tag( const xAOD::Jet& jet ) const override;

    private:

      /// Flag to indicate if Ntrk is used
      bool m_useNtrk;

      /// Store functional form of cuts
      std::string m_strD2Cut;
      std::string m_strNtrkCut;
      std::string m_strCNNCut;
      std::string m_strDisCoCut;

      /// Functions that are configurable for specific cut values
      std::unique_ptr<const TF1> m_funcD2Cut;
      std::unique_ptr<const TF1> m_funcNtrkCut;
      std::unique_ptr<const TF1> m_funcCNNCut;
      std::unique_ptr<const TF1> m_funcDisCoCut;

      /// Decorators
      std::unique_ptr< const SG::AuxElement::Decorator<float> > m_dec_d2cut;
      std::unique_ptr< const SG::AuxElement::Decorator<float> > m_dec_ntrkcut;
      std::unique_ptr< const SG::AuxElement::Decorator<float> > m_dec_cnncut;      
      std::unique_ptr< const SG::AuxElement::Decorator<float> > m_dec_discocut;      
      std::unique_ptr< const SG::AuxElement::Decorator<float> > m_dec_cnn;
      std::unique_ptr< const SG::AuxElement::Decorator<float> > m_dec_disco;

      // DisCoJet helper
      ToolHandle<ISmoothedWZDisCoJetTagger> m_DisCoJetHelper {"", this};
      bool m_LoadCNNTagger;
      bool m_UseCNNTagger;
      bool m_UseDisCoTagger;

  };

#endif
