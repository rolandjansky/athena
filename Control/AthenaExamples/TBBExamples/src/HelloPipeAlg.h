// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOPIPEALG_H
#define ATHEXHELLOWORLD_HELLOPIPEALG_H 1

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include <utility>
#include <map>

#include "DataModel/DataVector.h"
#include "tbb/pipeline.h"

/////////////////////////////////////////////////////////////////////////////
/** @class HelloPipeAlg
 *  @brief an algorithm that sets up a TBB pipeline of AlgTools and runs it
 *  details on TBB pipeline at http://threadingbuildingblocks.org/codesamples.php#pipeline
 *
 * The interest of a pipeline may reside in the ability to keep serial mode
 * running with small tasks (in terms of memory footprint) 
 *
 * @param NTokens number of tokens flowing through the pipeline (per event)
 * @param FilterSerial controls if processing filters run serially
 *
 * @author Paolo Calafiura - ATLAS Collaboration
 */
 
class IHelloTool;

class HelloPipeAlg : public AthAlgorithm {
public:
  HelloPipeAlg( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode beginRun();
  StatusCode endRun();
  
private:
  int    m_nTokens;  ///property
  bool   m_filterSerial; ///property

  ToolHandle< IHelloTool > m_myPrivateHelloTool;
  ToolHandle< IHelloTool > m_myPublicHelloTool;
  
  DataVector<tbb::filter> m_pipeFilters;
  tbb::pipeline m_pipeline;
};

#endif // ATHEXHELLOWORLD_HELLOPIPEALG_H
