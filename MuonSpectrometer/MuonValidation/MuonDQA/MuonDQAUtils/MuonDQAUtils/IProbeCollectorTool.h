/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMSProbeCollectorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IProbeCollectorTool_H
#define IProbeCollectorTool_H

#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_IProbeCollectorTool("IProbeCollectorTool", 1, 0);

class IProbeCollectorTool : virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID( ) ;

  // enter declaration of your interface-defining member functions here
  virtual StatusCode createProbeCollection() = 0; // const = 0;
};

inline const InterfaceID& IProbeCollectorTool::interfaceID()
{ 
  return IID_IProbeCollectorTool; 
}

#endif 
