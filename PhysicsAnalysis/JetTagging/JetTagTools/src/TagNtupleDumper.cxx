/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterNNTool.h
/// Package : BTagTools
/// Author  : Dan Guest
/// Created : March 2016
///
/// DESCRIPTION:
/// Class to dump a ROOT file with tagger info
///
/////////////////////////////////////////////////////////////////////////////

#include "JetTagTools/TagNtupleDumper.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TTree.h"

#include <utility>
#include <set>

namespace Analysis {

  TagNtupleDumper::TagNtupleDumper(const std::string& name, const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_hist_svc("THistSvc", name)
  {
    // python binding
    declareInterface<IMultivariateJetTagger>(this);

    // setup stream
    declareProperty("Stream", m_stream = "FTAG");
  }

  TagNtupleDumper::~TagNtupleDumper() {
    for (auto& pair: m_features) {
      delete pair.second;
    }
    // no idea how the hist service handles trees, do I have to delete them?
    // for (auto& tree: m_trees) {
    //   delete tree.second;
    // }
  }

  StatusCode TagNtupleDumper::initialize() {
    CHECK( m_hist_svc.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode TagNtupleDumper::finalize() { // all taken care of in destructor
    return StatusCode::SUCCESS;
  }
  typedef std::map<std::string,double> var_map;
  void TagNtupleDumper::assignProbability(xAOD::BTagging*,
                                          const var_map &inputs,
                                          const std::string &jetauthor){
    if (m_trees.count(jetauthor) == 0) {
      m_trees[jetauthor] = new TTree(jetauthor.c_str(), "who cares");
      m_hist_svc->regTree("/" + m_stream + "/" + jetauthor,
                          m_trees.at(jetauthor));
    }
    for (const auto& in: inputs) {
      const auto idx = std::make_pair(in.first, jetauthor);
      // add this branch if it doesn't exist
      if (m_features.count(idx) == 0) {
        m_features[idx] = new float;
        m_trees.at(jetauthor)->Branch(in.first.c_str(), m_features.at(idx));
        ATH_MSG_INFO("Added Branch " + jetauthor + ": " + in.first );
      }
      *m_features.at(idx) = in.second;
    }
    // fill tree
    m_trees.at(jetauthor)->Fill();
  } //end assign_probability
}

