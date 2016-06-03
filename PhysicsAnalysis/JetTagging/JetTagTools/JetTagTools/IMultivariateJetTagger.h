/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_IMULTIVARIATEJETTAGGER_C
#define BTAGTOOL_IMULTIVARIATEJETTAGGER_C

/******************************************************
    @class IMultivariateJetTagger
    Package : JetTagTools
    Created : Oct 2015
    DESCRIPTION:
    General interface for multivariate based classifiers. Used as the base
    for DL1, but needn't be NN based.

    @author Dan Guest, Marie Lanfermann

********************************************************/
#include "GaudiKernel/IAlgTool.h"
#include <map>
#include <string>

namespace xAOD {
  class BTagging_v1;
  typedef BTagging_v1 BTagging;
}


namespace Analysis {
  static const InterfaceID
    IID_IMultivariateJetTagger("Analysis::IMultivariateJetTagger", 1, 0);

  class IMultivariateJetTagger : virtual public IAlgTool
  {
  public:
    virtual ~IMultivariateJetTagger(){};

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    virtual void assignProbability(xAOD::BTagging* BTag, const std::map<std::string,double>& inputs, const std::string& jetauthor)=0;

    static const InterfaceID& interfaceID(){
      return IID_IMultivariateJetTagger;
    };
  };
}
#endif // BTAGTOOTL_IMULTIVARIATEJETTAGGER_C
