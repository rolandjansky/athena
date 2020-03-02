// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_NOOPALGORITHM_H
#define ATHEXJOBOPTIONS_NOOPALGORITHM_H 1

#include "AthenaBaseComps/AthAlgorithm.h"


/////////////////////////////////////////////////////////////////////////////

class NoopAlgorithm : public AthAlgorithm {
public:
   NoopAlgorithm( const std::string& name, ISvcLocator* svcloc );

   StatusCode initialize();        // called once, at start of job
   StatusCode reinitialize();      // can be called anytime after init
   StatusCode execute();           // called every event
   StatusCode finalize();          // called once, at end of job

private:
// services to hold on to
};

#endif // !ATHEXJOBOPTIONS_NOOPALGORITHM_H
