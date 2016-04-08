/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_IHELLOTOOL_H
#define ATHEXHELLOWORLD_IHELLOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include <string>

class IHelloTool : virtual public IAlgTool {
public:
  virtual int addToIt(int soFar) = 0;
  virtual StatusCode saySomething(std::string& message) = 0;
   static const InterfaceID& interfaceID() {
     static const InterfaceID _IHelloToolID( "IHelloTool", 1, 0 );
     return _IHelloToolID;
   }
};
#endif // !ATHEXHELLOWORLD_IHELLOTOOL_H
