// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_TOOLUSINGTOOL_H
#define ATHEXJOBOPTIONS_TOOLUSINGTOOL_H 1

#include "AthExJobOptions/IVirtualTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>


/////////////////////////////////////////////////////////////////////////////

class ToolUsingTool : virtual public IVirtualTool, virtual public AthAlgTool {
public:
   ToolUsingTool( const std::string&, const std::string&, const IInterface* );

// to allow access to the IVirtualTool interface
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

// setup/teardown functions, similar like those for Algorithm/Service
   StatusCode initialize();
   StatusCode finalize();

// the magic method this tool provides
   virtual StatusCode performTask( double& result );

public:
// to resolve possible conflicts with IProperty::interfaceID()
   static const InterfaceID& interfaceID() { return IVirtualTool::interfaceID(); }

private:
   double m_factor;

   ToolHandle< IVirtualTool > m_pubTool;
   ToolHandle< IVirtualTool > m_privTool;
   ToolHandleArray< IVirtualTool > m_toolArray;
};

#endif // !ATHEXJOBOPTIONS_TOOLUSINGTOOL_H
