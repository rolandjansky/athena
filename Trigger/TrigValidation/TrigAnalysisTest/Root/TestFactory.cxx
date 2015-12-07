/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Simple factory that will generate the test
// object given a name.
//

#include "TrigAnalysisTest/TestFactory.h"
#include "TrigAnalysisTest/BasicTriggerFired.h"
#include "TrigAnalysisTest/Run1BStoxAODTrigger.h"

#include <stdexcept>
#include <sstream>

using namespace std;

namespace TrigAnalysisTest {

  // Fetch a test. If name is not known then throw an exception.
  ITest *GetTrigAnalysisTest (const string &name) {
    if (name == "BasicTriggerFired") {
      return new BasicTriggerFired();
    } else if (name == "Run1BStoxAODTrigger") {
      return new Run1BStoxAODTrigger();
    } else {
      // If we are here, then we don't know what kind of test the framework
      // is asking for. So bomb.
      ostringstream errtxt;
      errtxt << "Trigger Analysis Test " << name << " is not known";
      throw runtime_error (errtxt.str().c_str());
    }
  }
}
