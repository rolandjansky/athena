// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETQGTAGGERBDT_H_
#define JETQGTAGGERBDT_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include <TSystem.h>
#include <TFile.h>
#include <TF1.h>
#include <TGraph.h>

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <set>
#include <list>
#include <vector>

namespace InDet {
  class IInDetTrackSelectionTool;
}

namespace CP {

  class JetQGTaggerBDT : public JSSTaggerBase {
    ASG_TOOL_CLASS0(JetQGTaggerBDT)

    public:

      //Default - so root can load based on a name
      JetQGTaggerBDT(const std::string& name);

      // Default - so we can clean up
      ~JetQGTaggerBDT();

      // Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      // IJetSelectorTool interface
      virtual Root::TAccept tag(const xAOD::Jet& jet) const override;

      // Retrieve BDT score
      float getScore(const xAOD::Jet& jet) const;

      // Update the jet substructure variables for each jet to use in BDT
      void getJetProperties(const xAOD::Jet& jet) const;

      virtual StatusCode finalize() override;

    private:

      // naming of tool
      std::string m_name;
      std::string m_APP_NAME;

      // TMVA tools
      std::unique_ptr<TMVA::Reader> m_bdtTagger;
      std::map<std::string,std::string> m_bdt_weights;
      std::string m_inputVariableSet;
      std::string m_BDTmethod;

      asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;

      // inclusive config file
      std::string m_configFile;
      std::string m_tmvaConfigFileName;
      std::string m_tmvaConfigFilePath;

      //string for cut function
      std::string m_strScoreCut;

      //TF1 for pT dependent cut
      std::unique_ptr<TF1> m_funcScoreCut;

      // variables for TMVA
      mutable float m_pt;
      mutable float m_eta;
      mutable float m_ntracks;
      mutable float m_trackwidth;
      mutable float m_trackC1;

      // bool to check whether variables are undefined
      mutable  bool m_undefInput;

      // string for decorating jets with DNN output
      std::string m_decorationName;

  };

} /* namespace CP */

#endif
