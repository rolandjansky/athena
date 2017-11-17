// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_HBBTaggerDNN_H_
#define BOOSTEDJETSTAGGERS_HBBTaggerDNN_H_

#include "AsgTools/AsgTool.h"
// #include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "JetInterface/IJetSelector.h"

namespace lwt {
  class LightweightGraph;
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
  virtual int keep(const xAOD::Jet& jet) const;
  double getScore(const xAOD::Jet& jet) const;

  protected:

   // the location where CVMFS files live
  std::string m_configFile;
  std::unique_ptr<lwt::LightweightGraph> m_lwnn;

};

#endif
