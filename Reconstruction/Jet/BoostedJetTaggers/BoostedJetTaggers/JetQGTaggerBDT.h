/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETQGTAGGERBDT_H_
#define JETQGTAGGERBDT_H_

#include "AsgTools/SlotSpecificObj.h"
#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "CxxUtils/checker_macros.h"

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

        /// IBoostedJetTagger interface
        virtual StatusCode tag(const xAOD::Jet& jet) const override;

      private:

        /// Retrieve BDT score
        float getScore( const xAOD::Jet& jet, asg::AcceptData &acceptData ) const;

        /// Update the jet substructure variables for each jet to use in BDT
        bool getJetProperties( const xAOD::Jet& jet, asg::AcceptData &acceptData ) const;

        bool getPrecomputedVariables( const xAOD::Jet& jet, asg::AcceptData &acceptData ) const;

        bool calculateVariables( const xAOD::Jet& jet, asg::AcceptData &acceptData ) const;

        bool isCorrectNumberOfTracks( int expectedNTracks, int nTracksFromGhostTracks ) const;

        /// Slot-specific TMVA tool and associated variables
        struct Tagger {
          std::unique_ptr<TMVA::Reader> tmva;
          float pt;
          float eta;
          float ntracks;
          float trackwidth;
          float trackC1;
        };

        mutable SG::SlotSpecificObj<Tagger> m_bdtTagger ATLAS_THREAD_SAFE;
        std::string m_BDTmethod;

        asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;

        // inclusive config file
        std::string m_tmvaConfigFileName;
        std::string m_tmvaConfigFilePath;

        int m_mode;

        /// WriteDecorHandle keys
        SG::WriteDecorHandleKey<xAOD::JetContainer> m_decScoreKey{this, "QGTaggerBDTScore", "QGTaggerBDTScore", "SG key for QGTaggerBDTScore"};

    };

} /* namespace CP */

#endif
