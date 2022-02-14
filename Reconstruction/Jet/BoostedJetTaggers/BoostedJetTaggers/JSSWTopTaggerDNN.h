/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JSSWTOPTAGGERDNN_H_
#define JSSWTOPTAGGERDNN_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"

class JSSWTopTaggerDNN :
  public JSSTaggerBase {
    ASG_TOOL_CLASS0(JSSWTopTaggerDNN)

    public:

      /// Constructor
      JSSWTopTaggerDNN( const std::string& name );

      /// Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      /// IJetSelectorTool interface
      virtual Root::TAccept& tag( const xAOD::Jet& jet ) const override;

    private:

      /// DNN tools
      std::unique_ptr<lwt::LightweightNeuralNetwork> m_lwnn;
      std::unique_ptr<lwt::LightweightGraph> m_lwnnGraph;

      /// Variables for DNN
      std::map<std::string, double> m_DNN_inputValues;

      /// Internal stuff to keep track of the output node for the NN                                                                                 
      std::vector<std::string> m_out_names;

      /// Retrieve score for a given DNN type (top/W)
      double getScore( const xAOD::Jet& jet ) const;

      /// Update the jet substructure variables for each jet to use in DNN
      std::map<std::string,double> getJetProperties( const xAOD::Jet& jet ) const;

  };

#endif
