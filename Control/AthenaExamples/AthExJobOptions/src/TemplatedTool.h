// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_TEMPLATEDTOOL_H
#define ATHEXJOBOPTIONS_TEMPLATEDTOOL_H 1

#include "AthExJobOptions/IVirtualTool.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>


/////////////////////////////////////////////////////////////////////////////

namespace ToolSpace {

template< class T >
class TemplatedTool : virtual public IVirtualTool, virtual public AthAlgTool {
public:
   TemplatedTool( const std::string&, const std::string&, const IInterface* );

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
};

}

#endif // !ATHEXJOBOPTIONS_TOOLUSINGTOOL_H
