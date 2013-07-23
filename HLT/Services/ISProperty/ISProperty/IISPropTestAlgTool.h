/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISPROPERTY_IISPROPTESTALGTOOL_H
#define ISPROPERTY_IISPROPTESTALGTOOL_H

// Include files
#include "GaudiKernel/IAlgTool.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IISPropTestAlgTool("IISPropTestAlgTool", 1 , 0); 

/** @class IISPropTestAlgTool IISPropTestAlgTool.h 
 *  Example of an Interface of a Algorithm Tool
 *
 *  @author Sebastian Boeser
*/
class IISPropTestAlgTool : virtual public IAlgTool {
 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IISPropTestAlgTool; }
  /// Do something
  virtual void  doIt() = 0;
};

#endif // ISPROPERTY_IISPROPTESTALGTOOL_H


