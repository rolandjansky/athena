/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOTOOLINTERFACES_IL1CALOMAPPINGTOOL_H
#define TRIGT1CALOTOOLINTERFACES_IL1CALOMAPPINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"

namespace LVL1 {

class IL1CaloMappingTool : virtual public IAlgTool {

public:
   DeclareInterfaceID (IL1CaloMappingTool, 1, 0);

   virtual bool mapping(int crate, int module, int channel,
                        double& eta, double& phi, int& layer) const = 0;
   virtual bool mapping(double eta, double phi, int layer,
                        int& crate, int& module, int& channel) const = 0;
      
};

} // end of namespace

#endif 
