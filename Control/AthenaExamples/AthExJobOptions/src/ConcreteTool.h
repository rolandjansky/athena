// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_CONCRETETOOL_H
#define ATHEXJOBOPTIONS_CONCRETETOOL_H 1

#include "AthExJobOptions/IVirtualTool.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>


/////////////////////////////////////////////////////////////////////////////

class ConcreteTool : virtual public IVirtualTool, virtual public AthAlgTool {
public:
   ConcreteTool( const std::string&, const std::string&, const IInterface* );

// to allow access to the IVirtualTool interface
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

// setup/teardown functions, similar like those for Algorithm/Service
   StatusCode initialize();
   StatusCode finalize();

// the magic method this tool provides
   StatusCode performTask( double& result );

public:
// to resolve possible conflicts with IProperty::interfaceID()
   static const InterfaceID& interfaceID() { return IVirtualTool::interfaceID(); }

private:
   double m_factor;
};

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
