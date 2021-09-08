/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETQGTAGGERBDT_H_
#define JETQGTAGGERBDT_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

namespace InDet {
  class IInDetTrackSelectionTool;
}

namespace CP {

  class JetQGTaggerBDT :
    public JSSTaggerBase {
      ASG_TOOL_CLASS0(JetQGTaggerBDT)

      public:

        /// Constructor
        JetQGTaggerBDT(const std::string& name);

        /// Run once at the start of the job to setup everything
        virtual StatusCode initialize() override;

        /// IJetSelectorTool interface
        virtual Root::TAccept& tag(const xAOD::Jet& jet) const override;

        /// Retrieve BDT score
        float getScore(const xAOD::Jet& jet) const;

      private:

        /// Update the jet substructure variables for each jet to use in BDT
        bool getJetProperties(const xAOD::Jet& jet) const;

        bool getPrecomputedVariables(const xAOD::Jet& jet) const;

        bool calculateVariables(const xAOD::Jet& jet) const;

        bool isCorrectNumberOfTracks(int expectedNTracks, int nTracksFromGhostTracks) const;

        /// TMVA tools
        std::unique_ptr<TMVA::Reader> m_bdtTagger;
        std::string m_BDTmethod;

        asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;

        // inclusive config file
        std::string m_tmvaConfigFileName;
        std::string m_tmvaConfigFilePath;

        // variables for TMVA
        mutable float m_pt;
        mutable float m_eta;
        mutable float m_ntracks;
        mutable float m_trackwidth;
        mutable float m_trackC1;

        int m_mode;

        /// Decorators
        std::unique_ptr< SG::AuxElement::Decorator< float > > m_dec_score;

    };

} /* namespace CP */

#endif
