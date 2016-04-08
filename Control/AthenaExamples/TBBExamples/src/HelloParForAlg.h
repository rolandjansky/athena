// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOPARFORALG_H
#define ATHEXHELLOWORLD_HELLOPARFORALG_H 1

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include <utility>
#include <map>

/////////////////////////////////////////////////////////////////////////////
/** @class HelloPipeAlg
 *  @brief an algorithm that sets up and run a TBB parallel_for
 *  details on TBB pipeline at http://threadingbuildingblocks.org/codesamples.php#Using_parallel_for
 *
 * parallel_for is the most promising TBB pattern to exploit mini-core 
 * platforms such as MIC. No framework services should be accessed in the
 * loop body which is implemented via a functor. Notice that the functor
 * is not generally handled a single iteration of the loop but a subrange 
 * of the loop.
 *
 * @param NIters number of iteration in for loop
 *
 * @author Paolo Calafiura - ATLAS Collaboration
 */


class IHelloTool;

class SayTools;

class HelloParForAlg : public AthAlgorithm {
public:
  HelloParForAlg( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode beginRun();
  StatusCode endRun();
  
private:
  friend class SayTools;
  int    m_nIters;

  ToolHandle< IHelloTool > m_myPrivateHelloTool;
  ToolHandle< IHelloTool > m_myPublicHelloTool;

};

#endif // ATHEXHELLOWORLD_HELLOPARFORALG_H
