/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IUSERACTIONSVC_H
#define G4ATLASINTERFACES_IUSERACTIONSVC_H

#include "GaudiKernel/IService.h"

/** @class IUserActionSvc IUserActionSvc.h "G4AtlasInterfaces/IUserActionSvc.h"
 *  
 *  Abstract interface to manage simulation user actions
 * 
 *  @author Andrea Di Simone
 *  @date   2015-01-13
 */

class IUserActionSvc : virtual public IService {
 public:
  IUserActionSvc() {}
  virtual ~IUserActionSvc() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IUserActionSvc( "IUserActionSvc" , 1 , 0 ) ;
    return IID_IUserActionSvc ;
  }

};

#endif
