/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_IVIRTUALTOOL_H
#define ATHEXJOBOPTIONS_IVIRTUALTOOL_H

#include "GaudiKernel/IAlgTool.h"


/////////////////////////////////////////////////////////////////////////////

class IVirtualTool : virtual public IAlgTool {
public:
   static const InterfaceID& interfaceID();

public:
   virtual StatusCode performTask( double& result ) = 0;
};

#endif // !ATHEXJOBOPTIONS_IVIRTUALTOOL_H
