/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestAlgToolInterface_H
#define TestAlgToolInterface_H

#include "GaudiKernel/IAlgTool.h"
class IDigiTestTool : virtual public IAlgTool{
public:
  virtual StatusCode processEvent() = 0;

  static const InterfaceID& interfaceID() {
    static const InterfaceID _IID( "IDigiTestTool", 1, 0 );
    return _IID;
  }

};

#endif
