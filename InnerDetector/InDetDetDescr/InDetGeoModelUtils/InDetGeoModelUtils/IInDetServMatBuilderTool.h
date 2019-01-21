/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IInDetServMatBuilderTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IInDetServMatBuilderTool_H
#define IInDetServMatBuilderTool_H

#include "GaudiKernel/IAlgTool.h"

#include <list>

namespace InDetDD {
  class ServiceVolume;
}

static const InterfaceID IID_IInDetServMatBuilderTool("IInDetServMatBuilderTool", 1, 0);


/// Tool for building services in SLHC geometry
class IInDetServMatBuilderTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID( ) ;
  
  virtual const std::vector<const InDetDD::ServiceVolume *> & getServices() = 0;  
};
  
inline const InterfaceID& IInDetServMatBuilderTool::interfaceID()
{
  return IID_IInDetServMatBuilderTool;
}
  
#endif //IInDetServMatBuilderTool_H
