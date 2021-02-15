/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JSSWTOPTAGGERANN_H_
#define JSSWTOPTAGGERANN_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"

class JSSWTopTaggerANN :
  public JSSTaggerBase {
    ASG_TOOL_CLASS0(JSSWTopTaggerANN)

    public:

      /// Constructor
      JSSWTopTaggerANN(const std::string& name);

      /// Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      /// Decorate single jet with tagging info
      virtual StatusCode tag(const xAOD::Jet& jet) const override;

    private:
      
      /// ANN tools
      std::unique_ptr<lwt::LightweightGraph> m_lwnn;
      std::map<std::string, std::map<std::string,double>> m_ANN_inputValues;   // variables for ANN

      /// Internal stuff to keep track of the output node for the NN
      std::vector<std::string> m_out_names;

      /// Retrieve score for a given ANN type (top/W)
      double getScore(const xAOD::Jet& jet) const;

      /// Update the jet substructure variables for each jet to use in ANN
      std::map<std::string, std::map<std::string,double>> getJetProperties(const xAOD::Jet& jet) const;

  };

#endif
