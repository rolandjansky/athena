// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_TESTSVCHANDLEARRAYALG_H
#define ATHEXJOBOPTIONS_TESTSVCHANDLEARRAYALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

class IVirtualSvc;

#include <string>
#include <vector>


/////////////////////////////////////////////////////////////////////////////

class TestSvcHandleArrayAlg : public AthAlgorithm {
public:
   TestSvcHandleArrayAlg( const std::string& name, ISvcLocator* svcloc );

   StatusCode initialize();        // called once, at start of job
   StatusCode reinitialize();      // can be called anytime after init
   StatusCode beginRun();          // called at begin of every run
   StatusCode execute();           // called every event
   StatusCode endRun();            // called at end of every run
   StatusCode finalize();          // called once, at end of job

private:
   typedef ServiceHandleArray<IVirtualSvc> IVirtualSvcs_t;
   IVirtualSvcs_t m_listOfSvcs;

};

#endif // !ATHEXJOBOPTIONS_TESTSVCHANDLEARRAYALG_H
