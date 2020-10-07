// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_DeepsetXbbTagger_H_
#define BOOSTEDJETSTAGGERS_DeepsetXbbTagger_H_

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetSelector.h"

#include <xAODTracking/VertexContainer.h>

namespace lwt {
  class LightweightGraph;
  class NanReplacer;
}

namespace DeepsetXbbTagger {

  struct TrackSelectorConfig
  {
    float pt_minumum = 1e3;
    float d0_maximum = 1;
    float z0_maximum = 1.5;
  };
  class TrackSelector;  
  class BTagSecVtxAugmenter;
  class DexterInputBuilder;
}

class Dexter :   public asg::AsgTool ,
                       virtual public IJetSelector
{
  ASG_TOOL_CLASS1(Dexter, IJetSelector )
public:
  Dexter(const std::string &name);
  ~Dexter();
  virtual StatusCode initialize() override final;

  // keep method is inherited from IJetSelector, it returns 0 if the
  // jet doesn't pass. The threshold is set via the tagThreshold
  // property.
  virtual int keep(const xAOD::Jet& jet) const override;

  // get the tagger output (only defined for single output networks)
  double getScore(const xAOD::Jet& jet) const;

  // get the output in the multi-output case
  std::map<std::string, double> getScores(const xAOD::Jet& jet) const;

  // this (and only this) method will add a decorator to the jet. The
  // name is set with the decorationNames property. If this is not
  // given the names are looked up from the the configuration file
  // under "outputs.decoration_map". If this is unspecified, fall back
  // to the names given in the lwtnn file.
  void decorate(const xAOD::Jet& jet) const;
  //
  // In some cases it's useful to add the decorations to a jet that
  // is not the one the inputs are coming from.
  void decorateSecond(const xAOD::Jet& ref, const xAOD::Jet& target) const;
  //
  // convenience function to get the decorator names
  std::set<std::string> decorationNames() const;

  // check how many subjets there are
  size_t n_subjets(const xAOD::Jet& jet) const;
  
  // Get PrimaryVertex
  xAOD::Vertex* getPrimaryVertex(const xAOD::VertexContainer*) const;

protected:
   // the location where CVMFS files live
  std::string m_neuralNetworkFile;
  std::string m_configurationFile;

  // secvtx collection name
  std::string m_secvtx_collection_name;

  // variable cleaner, replace Nan and Inf with default values
  typedef std::unique_ptr<lwt::NanReplacer> Cleaner;
  std::vector<std::pair<std::string, Cleaner > > m_var_cleaners;

  // neural network and feeder class
  std::unique_ptr<lwt::LightweightGraph> m_lwnn;
  std::unique_ptr<DeepsetXbbTagger::DexterInputBuilder> m_input_builder;

  // threshold to cut on for keep()
  // default 1000000000 - but the user must set this to use the tool sensibly
  double m_tag_threshold;

  // internal stuff to keep track of the output node for the NN
  std::vector<std::string> m_output_value_names;

  // if no decoration name is given we look it up from the
  // configuration output name
  std::map<std::string, std::string> m_decoration_names;
  std::vector<SG::AuxElement::Decorator<double> > m_decorators;

  // Record the offset of each input variable
  std::map<std::string, double> m_offsets;

};

#endif
