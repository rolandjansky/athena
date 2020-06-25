/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOTOOLINTERFACES_IL1CALOMAPPINGTOOL_H
#define TRIGT1CALOTOOLINTERFACES_IL1CALOMAPPINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"

namespace LVL1 {

static const InterfaceID IID_IL1CaloMappingTool("LVL1::IL1CaloMappingTool", 1, 0);

class IL1CaloMappingTool : virtual public IAlgTool {

 public:
   static const InterfaceID& interfaceID();

   virtual bool mapping(int crate, int module, int channel,
                        double& eta, double& phi, int& layer) = 0;
   virtual bool mapping(double eta, double phi, int layer,
                        int& crate, int& module, int& channel) = 0;
      
};

inline const InterfaceID& IL1CaloMappingTool::interfaceID()
{ 
  return IID_IL1CaloMappingTool;
}

} // end of namespace

#endif 
