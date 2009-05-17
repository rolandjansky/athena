// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_TOPALGORITHM_H
#define ATHEXJOBOPTIONS_TOPALGORITHM_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthExJobOptions/KeyMixin.h"

class StoreGateSvc;
class IVirtualTool;
class IVirtualSvc;

#include <string>
#include <vector>


/////////////////////////////////////////////////////////////////////////////

class TopAlgorithm : public AthAlgorithm, private KeyMixin {
public:
   TopAlgorithm( const std::string& name, ISvcLocator* svcloc );

   StatusCode initialize();        // called once, at start of job
   StatusCode reinitialize();      // can be called anytime after init
   StatusCode beginRun();          // called at begin of every run
   StatusCode execute();           // called every event
   StatusCode endRun();            // called at end of every run
   StatusCode finalize();          // called once, at end of job

private:
   double m_rndmFactor;

   typedef ToolHandle<IVirtualTool> IVirtualTool_t;
   typedef ToolHandleArray<IVirtualTool> IVirtualTools_t;

   IVirtualTool_t m_tool;
   IVirtualTool_t m_publicTool;
   IVirtualTools_t m_listOfPrivateTools;
   IVirtualTools_t m_listOfPublicTools;

   typedef ServiceHandle<IVirtualSvc> IVirtualSvc_t;
   IVirtualSvc_t m_svc;

   // testing bug #31571
   // https://savannah.cern.ch/bugs/?31571
   IVirtualTool_t m_emptyPrivateTool;
   IVirtualTool_t m_emptyPublicTool;
};

#endif // !ATHEXJOBOPTIONS_TOPALGORITHM_H
