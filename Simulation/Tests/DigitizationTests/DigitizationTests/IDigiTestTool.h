/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestAlgToolInterface_H
#define TestAlgToolInterface_H

#include "GaudiKernel/IAlgTool.h"
class IDigiTestTool : virtual public IAlgTool{
public:
  virtual StatusCode processEvent() = 0;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IDigiTestTool, 1, 0);
};

#endif
