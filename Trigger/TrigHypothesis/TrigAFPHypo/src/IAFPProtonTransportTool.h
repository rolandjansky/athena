/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGAFPHYPO_IAFPPROTONTRANSPORTTOOL_H
#define TRIGAFPHYPO_IAFPPROTONTRANSPORTTOOL_H

#include "GaudiKernel/IAlgTool.h"

class IAFPProtonTransportTool : virtual public IAlgTool {
 public:
  
  static const InterfaceID& interfaceID() { 
    static const InterfaceID id("IAFPProtonTransportTool", 1 , 0);
    return id; 
  }

  virtual StatusCode load() = 0;
  virtual void setParamFile(std::string paramFile) = 0;

  virtual double x(double x0, double y0, double z0, double sx0, double sy0, double E) const = 0;

};

#endif
