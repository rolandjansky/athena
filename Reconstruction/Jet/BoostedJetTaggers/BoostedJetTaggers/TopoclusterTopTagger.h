/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPOCLUSTERTOPTAGGER_H_
#define TOPOCLUSTERTOPTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "BoostedJetTaggers/TopoclusterTransform.h"

#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"

class TopoclusterTopTagger :
  public JSSTaggerBase {
    ASG_TOOL_CLASS0(TopoclusterTopTagger)

    public:

      enum Result {

        OutOfRangeHighPt = -3,
        OutOfRangeLowPt = -2,
        OutOfRangeEta = -1,
        InvalidJet = 0,

        AllPassed = 1,
        MassPassMVAPass = 1,

        MassPassMVAFail     = 2,
        LowMassFailMVAPass  = 4,
        LowMassFailMVAFail  = 8,
        HighMassFailMVAPass = 16,
        HighMassFailMVAFail = 32,

      };

      //Default - so root can load based on a name
      TopoclusterTopTagger(const std::string& name);

      // Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      // IJetSelectorTool interface
      virtual Root::TAccept& tag(const xAOD::Jet& jet) const override;

      // Preprocess the jet constituents by applying transformations
      void preprocess(std::map<std::string,double> &clusters, xAOD::Jet jet) const;

      // Retrieve score for a given DNN type (top/W)
      double getScore(const xAOD::Jet& jet) const;

      // Update the jet constituents for this jet to use in DNN
      std::map<std::string,double> getJetConstituents(const xAOD::Jet& jet) const;

    private:

      // DNN tools
      std::unique_ptr<lwt::LightweightNeuralNetwork> m_lwnn;
      std::map<std::string, double> m_DNN_inputValues;   // variables for DNN

  };

#endif
