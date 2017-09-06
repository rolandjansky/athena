/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MinixAODAnalysis.h 683667 2015-07-17 02:24:59Z morrisj $
#ifndef ANALYSISTOP_TOPEXAMPLES_MINIXAODANALYSIS_H
#define ANALYSISTOP_TOPEXAMPLES_MINIXAODANALYSIS_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief MinixAODAnalysis
 *
 * $Revision: 683667 $
 * $Date: 2015-07-17 04:24:59 +0200 (Fri, 17 Jul 2015) $
 *
 *
 **/

#include <memory>
#include <unordered_map>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
  class PlotManager;
}
class TFile;

#include "TopCPTools/TopToolStore.h"
#include "TopCorrections/ScaleFactorCalculator.h"
#include "TopEvent/TopEventMaker.h"

namespace top{
  class MinixAODAnalysis : public asg::AsgTool {
    public:
      explicit MinixAODAnalysis( const std::string& name );
      virtual ~MinixAODAnalysis(){}
      
      // Delete Standard constructors
      MinixAODAnalysis(const MinixAODAnalysis& rhs) = delete;
      MinixAODAnalysis(MinixAODAnalysis&& rhs) = delete;
      MinixAODAnalysis& operator=(const MinixAODAnalysis& rhs) = delete; 
      
      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();
      
    private:
      std::shared_ptr<top::TopConfig> m_config;
      
      std::unique_ptr<top::TopToolStore> m_topTools;
      std::unique_ptr<top::ScaleFactorCalculator> m_sfCalc;      
      std::unique_ptr<top::TopEventMaker> m_evtMaker;
      
      std::string m_outputFileName;
      ::TFile* m_outputFile;
      
      // Analysis
      bool m_firstEvent;
      std::string m_sampleName;
      std::unordered_map<std::string,std::unordered_map<std::string,top::PlotManager*>> m_plots;  
      typedef std::unordered_map<std::string,std::unordered_map<std::string,top::PlotManager*>>::const_iterator Itr;
      typedef std::unordered_map<std::string,top::PlotManager*>::const_iterator Itr2;

  };
}
#endif