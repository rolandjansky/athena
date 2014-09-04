/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AraTool_IARAALGTOOL_H
#define AraTool_IARAALGTOOL_H

#include "GaudiKernel/IAlgTool.h"


/////////////////////////////////////////////////////////////////////////////

class IAraAlgTool : virtual public IAlgTool {
public:
   static const InterfaceID& interfaceID();
};

#endif // !AraTool_IARAALGTOOL_H
