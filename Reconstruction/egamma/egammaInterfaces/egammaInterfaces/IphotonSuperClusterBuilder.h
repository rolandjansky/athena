/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IPHOTONSUPERCLUSTERBUILDER_H
#define EGAMMAINTERFACES_IPHOTONSUPERCLUSTERBUILDER_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IphotonSuperClusterBuilder("IphotonSuperClusterBuilder", 1, 0);

class IphotonSuperClusterBuilder : virtual public IAlgTool
{

 public:
  /** @brief Virtual destructor*/
  virtual ~IphotonSuperClusterBuilder() {};
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize()   = 0;
  virtual StatusCode execute()    = 0; 
};

inline const InterfaceID& IphotonSuperClusterBuilder::interfaceID()
{
  return IID_IphotonSuperClusterBuilder;
}

#endif
