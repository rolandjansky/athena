/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTHYPOTOOL_H
#define TRIGUPGRADETEST_TESTHYPOTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// TrigUpgradeTest includes
#include "./ITestHypoTool.h"

namespace HLTTest {

/**
 * @class $(klass)s
 * @brief 
 **/

class TestHypoTool
  : virtual public ::HLTTest::ITestHypoTool,
            public ::AthAlgTool
{ 

 public: 

  TestHypoTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~TestHypoTool(); 

  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

 private: 

  TestHypoTool();

}; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTHYPOTOOL_H
