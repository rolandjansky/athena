/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDWZDISCOJETTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDWZDISCOJETTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "BoostedJetTaggers/ISmoothedWZDisCoJetTagger.h"
#include "AthOnnxRuntimeBJT/JSSMLTool.h"

class SmoothedWZDisCoJetTagger :
  public ISmoothedWZDisCoJetTagger, public JSSTaggerBase {
  ASG_TOOL_CLASS(SmoothedWZDisCoJetTagger, ISmoothedWZDisCoJetTagger)
  
    public:

      /// Constructor
      SmoothedWZDisCoJetTagger( const std::string& name );

      /// Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      /// IJetSelectorTool interface
      virtual Root::TAccept& tag( const xAOD::Jet& jet ) const override;

      // CNN tagger
      float GetCNNScore(const xAOD::Jet& jet) const override;

      // DisCoJet tagger
      float GetDisCoDNNScore(const xAOD::Jet& jet) const override;
      std::map<std::string, double> GetJSSVars(const xAOD::Jet& jet) const override;
      StatusCode ReadScaler() override;

      std::map<std::string, std::vector<double>> m_scaler;


    private:

      // CNN tagger 
      bool m_LoadCNNTagger;
      bool m_UseCNNTagger;

      // WARNING: this is not thread-safe and
      // TODO   : this needs to be fixed before being accepted into master
      mutable ToolHandle<AthONNX::IJSSMLTool> m_MLBosonTagger {"", this};
      
      // DisCo tagger
      bool m_UseDisCoTagger;

      // WARNING: this is not thread-safe and
      // TODO   : this needs to be fixed before being accepted into master
      mutable ToolHandle<AthONNX::IJSSMLTool> m_MLBosonTagger_DisCo {"", this};

  };

#endif
