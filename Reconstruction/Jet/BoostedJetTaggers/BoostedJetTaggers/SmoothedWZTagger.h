/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"

class SmoothedWZTagger :
  public JSSTaggerBase {
    ASG_TOOL_CLASS0(SmoothedWZTagger)
  
    public:

      /// Constructor
      SmoothedWZTagger( const std::string& name );

      /// Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      /// Decorate single jet with tagging info
      virtual StatusCode tag( const xAOD::Jet& jet ) const override;

    private:

      /// Flag to indicate if Ntrk is used
      bool m_useNtrk;

      /// Store functional form of cuts
      std::string m_strD2Cut;
      std::string m_strNtrkCut;

      /// Functions that are configurable for specific cut values
      std::unique_ptr<TF1> m_funcD2Cut;
      std::unique_ptr<TF1> m_funcNtrkCut;

      /// WriteDecorHandle keys
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_decPassD2Key{this, "PassD2Name", "PassD2", "SG key for PassD2"};
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_decPassNtrkKey{this, "PassNtrkName", "PassNtrk", "SG key for PassNtrk"};

      SG::WriteDecorHandleKey<xAOD::JetContainer> m_decCutD2Key{this, "CutD2Name", "Cut_D2", "SG key for Cut_D2"};
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_decCutNtrkKey{this, "CutNtrkName", "Cut_Ntrk", "SG key for Cut_Ntrk"};
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_decAcceptKey{this, "acceptName", "accept", "SG key for accept"};

      /// ReadDecorHandle keys
      SG::ReadDecorHandleKey<xAOD::JetContainer> m_readUngroomedLinkKey{this, "ParentName", "Parent", "SG key for Parent"};

  };

#endif
