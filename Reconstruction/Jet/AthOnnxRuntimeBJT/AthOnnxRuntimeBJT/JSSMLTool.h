// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#ifndef ATHONNXRUNTIMEBJT_JSSMLTOOL_H
#define ATHONNXRUNTIMEBJT_JSSMLTOOL_H

#include "IJSSMLTool.h"
#include "AsgTools/AsgTool.h"

// Framework include(s).
//#include "AnaAlgorithm/AnaAlgorithm.h"

// ONNX Runtime include(s).
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

// System include(s).
#include <memory>
#include <string>
#include <iostream> 
#include <fstream>
#include <arpa/inet.h>
#include <numeric>

// root
#include "TH2.h"

// xAOD
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"

namespace AthONNX {

   /// Tool using the ONNX Runtime C++ API 
   /// to retrive constituents based model for boson jet tagging
   ///
   /// this is inspired from the general athena example here:
   /// https://gitlab.cern.ch/atlas/athena/-/blob/21.2/Control/AthenaExamples/AthExOnnxRuntime/AthExOnnxRuntime/CxxApiAlgorithm.h
   ///
   /// @author Antonio Giannini <antonio.giannini@cern.ch>
   ///
class JSSMLTool 
  : public asg::AsgTool, 
    virtual public IJSSMLTool {
  ASG_TOOL_CLASS(JSSMLTool, IJSSMLTool)

  public:
    JSSMLTool (const std::string& name);

    /// Function initialising the tool
    virtual StatusCode initialize() override;
    /// Function executing the tool for a single event
    virtual StatusCode retriveConstituentsScore() override;
    virtual StatusCode retrieveHighLevelScore() override;

    // basic tool functions
    std::vector<std::vector<float>> ReadJetImagePixels( std::vector<TH2D> Images ) const;
    std::vector<int> ReadJetImageLabels() const;
    std::vector<std::vector<float>> ReadJSSInputs();
    std::vector<int> ReadJSSInputsLabels() const;

    // extra methods
    TH2D MakeJetImage(TString TagImage, const xAOD::Jet* jet, std::vector<xAOD::JetConstituent> constituents) override;
    void PrintInfo() override;
    void PrintInfoImages() override;
    void ResetImages() override;
    double GetCNNScore() override;
    void SetJSSInputs(const std::map<std::string, double> JSSVars) override;
    double GetDNNScore() override;
    StatusCode SetScaler(std::map<std::string, std::vector<double>> scaler) override;

      /// @}
    std::unique_ptr< Ort::Session > m_session;
    std::unique_ptr< Ort::Env > m_env;

    std::map<TString, TH2D> m_MapImages;
    double m_CNNScore;
    std::map<std::string, double> m_JSSVars;
    std::map<std::string, std::vector<double>> m_scaler;
    std::map<int, std::string> m_JSSInputMap;
    double m_DNNScore;

   private:

    /// Name of the model file to load
      std::string m_modelFileName;
      std::string m_pixelFileName;
      std::string m_labelFileName;
      int m_testSample;

     // input node info
     std::vector<int64_t> m_input_node_dims;
     size_t m_num_input_nodes;
     std::vector<const char*> m_input_node_names;

     // output node info
     std::vector<int64_t> m_output_node_dims;
     size_t m_num_output_nodes;
     std::vector<const char*> m_output_node_names;

     // some configs
     int m_nbins_eta, m_nbins_phi;
     double m_min_eta, m_max_eta, m_min_phi, m_max_phi;
     int m_ncolors;
     bool m_dorscaling;
     double m_rscaling_p0, m_rscaling_p1;
     int m_nvars;

   }; // class JSSMLTool

} // namespace AthONNX

#endif // ATHONNXRUNTIMEBJT_JSSMLTOOL_H
