// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_TOPALGORITHM_H
#define ATHEXJOBOPTIONS_TOPALGORITHM_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "KeyMixin.h"

class IVirtualTool;
class IVirtualSvc;

#include <string>
#include <vector>


/////////////////////////////////////////////////////////////////////////////

class TopAlgorithm : public AthAlgorithm, private KeyMixin {
public:
   TopAlgorithm( const std::string& name, ISvcLocator* svcloc );

   virtual StatusCode initialize() override;        // called once, at start of job
   virtual StatusCode reinitialize() override;      // can be called anytime after init
   virtual StatusCode execute() override;           // called every event
   virtual StatusCode finalize() override;          // called once, at end of job

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
