/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IUserLimitsTool_H
#define G4ATLASINTERFACES_IUserLimitsTool_H

#include "GaudiKernel/IAlgTool.h"

/** @class IUserLimitsTool IUserLimitsTool.h "G4AtlasInterfaces/IUserLimitsTool.h"
 *
 *
 *
 */

class IUserLimitsTool : virtual public IAlgTool {
 public:
  virtual ~IUserLimitsTool() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IUserLimitsTool( "IUserLimitsTool" , 1 , 0 ) ;
    return IID_IUserLimitsTool ;
  }

};

#endif
