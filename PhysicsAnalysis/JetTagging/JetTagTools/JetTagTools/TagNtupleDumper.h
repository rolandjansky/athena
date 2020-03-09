/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include <mutex>

class TTree;
// class TFile;

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CxxUtils/checker_macros.h"

namespace Analysis {

  class TagNtupleDumper : public extends<AthAlgTool, IMultivariateJetTagger>
  {
  public:
    TagNtupleDumper(const std::string& name,
                    const std::string& n,
                    const IInterface*);

    virtual ~TagNtupleDumper();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual
    void assignProbability(xAOD::BTagging* BTag,
                           const std::map<std::string,double>& inputs,
                           const std::string& jetauthor) const override;

  private:
    ServiceHandle<ITHistSvc> m_hist_svc;
    std::string m_stream;

    mutable std::map< std::pair<std::string, std::string>, float* > m_features ATLAS_THREAD_SAFE;
    mutable std::map<std::string, TTree*> m_trees ATLAS_THREAD_SAFE;
    // Serialize access.
    mutable std::mutex m_mutex ATLAS_THREAD_SAFE;

  }; // end class

}// end Analysis namespace

#endif
