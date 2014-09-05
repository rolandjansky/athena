/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDPROXY_H
#define TILECONDITIONS_ITILECONDPROXY_H

#include "GaudiKernel/IAlgTool.h"

template<typename T>
class ITileCondProxy : virtual public IAlgTool {
 public:

  virtual ~ITileCondProxy() {}

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ITileCondProxy("ITileCondProxy", 1 , 0);
    return IID_ITileCondProxy; 
  }
  
  virtual const T* getCalibDrawer(unsigned int drawerIdx) const = 0;

};

#endif
