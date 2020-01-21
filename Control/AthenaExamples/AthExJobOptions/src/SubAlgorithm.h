// -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
   StatusCode start();             // called after initialize
   StatusCode execute();           // called every event
   StatusCode stop();              // called before finalize
   StatusCode finalize();          // called once, at end of job

private:
// add toolhandle ...
   std::vector< std::string > m_selections;
};

#endif // !ATHEXJOBOPTIONS_SUBALGORITHM_H
