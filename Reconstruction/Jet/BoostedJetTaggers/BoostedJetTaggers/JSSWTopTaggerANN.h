/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JSSWTOPTAGGERANN_H_
#define JSSWTOPTAGGERANN_H_

#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"
#include "ParticleJetTools/JetTruthLabelingTool.h"

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"
#include "lwtnn/lightweight_nn_streamers.hh"
#include "lwtnn/NanReplacer.hh"

#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

#include <TF1.h>

#include <fstream>

class JSSWTopTaggerANN:  public JSSTaggerBase {
  ASG_TOOL_CLASS0(JSSWTopTaggerANN)
  
  public:
  
  //Default - so root can load based on a name
  JSSWTopTaggerANN(const std::string& name);
  
  // Default - so we can clean up
  ~JSSWTopTaggerANN();
  
  // Run once at the start of the job to setup everything
  virtual StatusCode initialize() override final;
  
  // IJSSTagger interface
  virtual Root::TAccept& tag(const xAOD::Jet& jet) const override final;
  
  // Retrieve score for a given ANN type (top/W)
  double getScore(const xAOD::Jet& jet) const;
  
  // Get scale factor
  std::pair<double,double> getWeight(const xAOD::Jet& jet) const;
  
  // Update the jet substructure variables for each jet to use in ANN
  std::map<std::string, std::map<std::string,double>> getJetProperties(const xAOD::Jet& jet) const;
  
private:
  std::string m_name;
  std::string m_APP_NAME;
  
  // for the tagging type
    enum TAGCLASS{Unknown, WBoson, TopQuark};

    // ANN tools
    std::unique_ptr<lwt::LightweightGraph> m_lwnn;
    std::map<std::string, std::map<std::string,double>> m_ANN_inputValues;   // variables for ANN

    // for internal usage
    mutable TAGCLASS m_TagClass;

    // bool to check whether variables are corrupt
    mutable bool m_undefInput;

    // parameters to store specific cut values
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strScoreCut;

    // functions that are configurable for specific cut values
    TF1* m_funcMassCutLow;
    TF1* m_funcMassCutHigh;
    TF1* m_funcScoreCut;

    // internal stuff to keep track of the output node for the NN
    std::vector<std::string> m_out_names;
       
    // truth labeling tool
    asg::AnaToolHandle<JetTruthLabelingTool> m_JetTruthLabelingTool; //!

    // decorators
    SG::AuxElement::Decorator<float> m_dec_mcutL;
    SG::AuxElement::Decorator<float> m_dec_mcutH;
    SG::AuxElement::Decorator<float> m_dec_scoreCut;
    SG::AuxElement::Decorator<float> m_dec_scoreValue;
};

#endif
