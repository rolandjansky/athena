/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** J.Montejo - 10/11/2015 - AthenaTestHarness **/

#ifndef AthenaTestHarness_AthenaTestHarness_H
#define AthenaTestHarness_AthenaTestHarness_H

// std stuff
#include <string>
#include <fstream>
#include <iostream>
#include "TrigAnalysisTest/ITest.h"

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

// core stuff
#include "GaudiKernel/ToolHandle.h"

// Trigger includes
#include "TrigDecisionTool/TrigDecisionTool.h"

using namespace std;
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;
using namespace TrigAnalysisTest;

/**
 * @class AthenaTestHarness
 * bSteer tests produced by TestFactory
 *
 * @author Javier Montejo  <jmontejo@cern.ch>  - CERN
 */
class AthenaTestHarness : public AthAlgorithm
{
  
 public:
  
  AthenaTestHarness(const std::string &name, ISvcLocator *pSvcLocator); //!< std Gaudi Algorithm constructor
  ~AthenaTestHarness();//!< std  deconstructor
  
  // IAlgorithm virtual methods to implement
  StatusCode initialize();//!< std Gaudi initialize method
  StatusCode execute(); //!< std Gaudi execute method -> use TrigDecisionTool to 'look' at the trigger data
  StatusCode finalize();//!< std Gaudi finalize method
  
 private:

  std::string m_trigDecisionKey; //!< SG key of the trigger data (TrigDecision object)
  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!< interface to use the trigger data: TriggerTool

	ITest *m_test;
	const std::string m_test_name;

};

#endif
