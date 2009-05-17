/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/IVirtualTool.h"


/////////////////////////////////////////////////////////////////////////////

static const InterfaceID IVirtualToolID_( "IVirtualTool", 1, 0 );

const InterfaceID& IVirtualTool::interfaceID() { 
   return IVirtualToolID_;
}
