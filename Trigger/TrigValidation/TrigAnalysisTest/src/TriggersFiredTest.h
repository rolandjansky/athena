/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** J.Montejo - 15/10/2015 - tests for TrigDecisionTool **/

#ifndef TriggersFiredTest_TriggersFiredTest_H
#define TriggersFiredTest_TriggersFiredTest_H

// std stuff
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

// core stuff
#include "GaudiKernel/ToolHandle.h"

// Trigger includes
#include "TrigDecisionTool/TrigDecisionTool.h"
//#include "TrigConfInterfaces/ITrigConfigSvc.h"
//#include "TrigConfxAOD/xAODConfigTool.h"

using namespace std;
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;

/**
 * @class TriggersFiredTest
 * bSimple algorithm to check fired triggers with TrigDecisionTool
 *
 * @author Javier Montejo  <jmontejo@cern.ch>  - CERN
 */
class TriggersFiredTest : public AthAlgorithm
{
  
 public:
  
  TriggersFiredTest(const std::string &name, ISvcLocator *pSvcLocator); //!< std Gaudi Algorithm constructor
  ~TriggersFiredTest();//!< std  deconstructor
  
  // IAlgorithm virtual methods to implement
  StatusCode initialize();//!< std Gaudi initialize method
  StatusCode execute(); //!< std Gaudi execute method -> use TrigDecisionTool to 'look' at the trigger data
  StatusCode finalize();//!< std Gaudi finalize method
  
 private:

  std::string m_trigDecisionKey; //!< SG key of the trigger data (TrigDecision object)
  bool m_first_event;
  std::vector<std::string> m_TestList;

  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!< interface to use the trigger data: TriggerTool

  unsigned int m_passed_l1;
  unsigned int m_passed_hlt;

};

#endif
