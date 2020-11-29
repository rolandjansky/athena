// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* **************************************************************************
                           DeepsetXbbTagger.h  -  Description
                             -------------------
    begin   : 20.10.11
    authors : Yuan-Tang Chou
    email   : yuan-tang.chou@cern.ch
    comments: Jet modfier tool to run and add btagging information from deep-set based Xbb tagger
   ***************************************************************************/



#ifndef BOOSTEDJETSTAGGERS_DeepsetXbbTagger_H_
#define BOOSTEDJETSTAGGERS_DeepsetXbbTagger_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "xAODTracking/VertexContainer.h"

namespace lwt {
  class LightweightGraph;
  class NanReplacer;
}

/** The namespace of all subclass for Dexter implementation */
namespace Dexter {

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

/**  \class DeepsetXbbTagger
    Jet modfier tool to run and add btagging information from 
    deep-set based Xbb tagger (DeXTer).
    The neuron network JSON contains the lwtnn model and 
    a separates JSON configuration for the input collection/features configuration.
    
    @author yuan-tang.chou@cern.ch

     */


class DeepsetXbbTagger :   
  public JSSTaggerBase {
  ASG_TOOL_CLASS0(DeepsetXbbTagger)

    public:

      DeepsetXbbTagger(const std::string &name);
      ~DeepsetXbbTagger();
  
      virtual StatusCode initialize() override;

      /** IJetSelectorTool interface */
      virtual Root::TAccept& tag(const xAOD::Jet& jet) const override;

      /** get the tagger output (only defined for single output networks) */
      double getScore(const xAOD::Jet& jet) const;

      /** get the output in the multi-output case */
      std::map<std::string, double> getScores(const xAOD::Jet& jet) const;

      /** this (and only this) method will add a decorator to the jet. The
        name is set with the decorationNames property. If this is not
        given the names are looked up from the the configuration file
        under "outputs.decoration_map". If this is unspecified, fall back
        to the names given in the lwtnn file. */
      void decorate(const xAOD::Jet& jet) const;

      /** In some cases it's useful to add the decorations to a jet that
        is not the one the inputs are coming from. */
      void decorateSecond(const xAOD::Jet& ref, const xAOD::Jet& target) const;

      /** convenience function to get the decorator names */
      std::set<std::string> decorationNames() const;

      /** check how many subjets there are */
      size_t n_subjets(const xAOD::Jet& jet) const;

      /** Get PrimaryVertex */
      xAOD::Vertex* getPrimaryVertex(const xAOD::VertexContainer*) const;

    protected:

      // the subjet selection
      float m_nSubjets;

      /** negative-tag mode */
      std::string m_negativeTagMode;

      /** secvtx collection name */
      std::string m_secvtx_collection_name;

      /** neural network and feeder class */
      std::unique_ptr<lwt::LightweightGraph> m_lwnn;
      std::unique_ptr<Dexter::DexterInputBuilder> m_input_builder;

      /** internal stuff to keep track of the output node for the NN  */
      std::vector<std::string> m_output_value_names;

      /** if no decoration name is given we look it up from the
        configuration output name  */ 
      std::map<std::string, std::string> m_decoration_names;
      std::vector<SG::AuxElement::Decorator<double> > m_decorators;

      /** Record the offset of each input variable */
      std::map<std::string, double> m_offsets;

  };

#endif
