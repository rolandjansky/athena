/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOLS_TAGNTUPLEDUMPER_C
#define BTAGTOOLS_TAGNTUPLEDUMPER_C

/******************************************************
    @class TagNtupleDumper
    Package : JetTagTools
    Created : March 2016
    DESCRIPTION:
    Package to dump ntuples containing the information used in the
    ML taggers
    @authors Dan Guest, Luke de Oliveira, Marie Lanfermann
********************************************************/
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IMultivariateJetTagger.h"

#include <vector>
#include <istream>
#include <map>
#include <utility>

class TTree;
// class TFile;

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

namespace Analysis {

  class TagNtupleDumper : public AthAlgTool, public IMultivariateJetTagger
  {
  public:
    TagNtupleDumper(const std::string& name,
                    const std::string& n,
                    const IInterface*);

    virtual ~TagNtupleDumper();
    StatusCode initialize();
    StatusCode finalize();

    void assignProbability(xAOD::BTagging* BTag,
                           const std::map<std::string,double>& inputs,
                           const std::string& jetauthor);

  private:
    ServiceHandle<ITHistSvc> m_hist_svc;
    std::map< std::pair<std::string, std::string>, float* > m_features;
    std::string m_stream;
    std::map<std::string, TTree*> m_trees;

  }; // end class

}// end Analysis namespace

#endif
