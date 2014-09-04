/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AraTool/IAraAlgTool.h"


/////////////////////////////////////////////////////////////////////////////

static const InterfaceID IAraAlgToolID_( "IAraAlgTool", 1, 0 );

const InterfaceID& IAraAlgTool::interfaceID() { 
   return IAraAlgToolID_;
}
