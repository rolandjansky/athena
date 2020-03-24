// This is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATH_RNNTOOL_H
#define ATH_RNNTOOL_H

/**********************************************************************************
 * @Package: PhysicsAnpProd
 * @Class  : RNNTool
 * @Author : Rustem Ospanov
 *
 * @Brief  : Tool to access RNN from lwtnn library
 * 
 **********************************************************************************/

// Tools
#include "PathResolver/PathResolver.h"

// Local
#include "IRNNTool.h"
#include "VarHolder.h"

// Athena
#include "AthenaBaseComps/AthAlgTool.h"

// External
#include "lwtnn/LightweightGraph.hh"

namespace Prompt
{
  class RNNTool : public AthAlgTool, public IRNNTool
  {
  /*
    RNN tool is based on the lwtnn package.
    1. It will take the inputs VarHolder objects and convert them into lwt::VectorMap format.
    2. lwtnn will use lwt::VectorMap format inputs with a given RNN weight json file to predict RNN scores.
    3. Then this tool will return std::map<std::string, double> object, which contains the name string of the RNN scores and their predictions.

  */
  public:
       
    RNNTool(const std::string &name,
	    const std::string &type,
	    const IInterface  *parent);

    virtual ~RNNTool() {}

    virtual StatusCode initialize(void);

    virtual std::map<std::string, double> computeRNNOutput(const std::vector<Prompt::VarHolder> &tracks);    

    virtual std::set<std::string> getOutputLabels() const { return m_outputLabels; }

  private:

    void AddVariable(const std::vector<Prompt::VarHolder> &tracks, unsigned var, std::vector<double> &values);

  private:    

    std::string                              m_configPathRNN;
    std::string                              m_configRNNVersion;
    std::string                              m_configRNNJsonFile;
    bool                                     m_debug;
    
    std::string                              m_inputSequenceName;
    unsigned                                 m_inputSequenceSize;
    
    std::set<std::string>                    m_outputLabels;

    std::unique_ptr<lwt::LightweightGraph>   m_graph;   
  }; 
} 

#endif 
