// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_SUBALGORITHM_H
#define ATHEXJOBOPTIONS_SUBALGORITHM_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>


/////////////////////////////////////////////////////////////////////////////

class SubAlgorithm : public AthAlgorithm {
public:
   SubAlgorithm( const std::string& name, ISvcLocator* svcloc );

   StatusCode initialize();        // called once, at start of job
   StatusCode reinitialize();      // can be called anytime after init
   StatusCode beginRun();          // called at begin of every run
   StatusCode execute();           // called every event
   StatusCode endRun();            // called at end of every run
   StatusCode finalize();          // called once, at end of job

private:
// add toolhandle ...
   std::vector< std::string > m_selections;
};

#endif // !ATHEXJOBOPTIONS_SUBALGORITHM_H
