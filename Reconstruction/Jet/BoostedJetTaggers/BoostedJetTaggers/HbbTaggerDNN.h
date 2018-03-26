// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_HBBTaggerDNN_H_
#define BOOSTEDJETSTAGGERS_HBBTaggerDNN_H_

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetSelector.h"

namespace lwt {
  class LightweightGraph;
}

namespace BoostedJetTaggers {
  class HbbInputBuilder;
}

class HbbTaggerDNN :   public asg::AsgTool ,
                       virtual public IJetSelector
{
  ASG_TOOL_CLASS1(HbbTaggerDNN, IJetSelector )
public:
  HbbTaggerDNN(const std::string &name);
  ~HbbTaggerDNN();
  StatusCode initialize();
  StatusCode finalize();

  // keep method is inherited from IJetSelector, it returns 0 if the
  // jet doesn't pass. The threshold is set via the tagThreshold
  // property.
  virtual int keep(const xAOD::Jet& jet) const;

  // get the tagger output.
  double getScore(const xAOD::Jet& jet) const;

  // this (and only this) method will add a decorator to the jet. The
  // name is set with the decorationName property.
  void decorate(const xAOD::Jet& jet) const;

  // check how many subjets there are
  size_t n_subjets(const xAOD::Jet& jet) const;

protected:
   // the location where CVMFS files live
  std::string m_neuralNetworkFile;
  std::string m_configurationFile;
  std::unique_ptr<lwt::LightweightGraph> m_lwnn;
  std::unique_ptr<BoostedJetTaggers::HbbInputBuilder> m_input_builder;

  // threshold to cut on for keep()
  // default 1000000000 - but the user must set this to use the tool sensibly
  double m_tag_threshold;

  // internal stuff to keep track of the output node for the NN
  std::string m_output_value_name;

  // if no decoration name is given we look it up from the
  // configuration output name
  std::string m_decoration_name;
  SG::AuxElement::Decorator<float> m_decorator;

};

#endif
