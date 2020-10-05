/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGGERTRANSLATORTOOL_H
#define ITRIGGERTRANSLATORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ITriggerTranslatorTool("ITriggerTranslatorTool", 1 , 0); 


//	The interface implemented by concrete Monitoring tools.
//      Concrete tools, derived from the MonitorToolBase base abstract
//	class are controlled via this interface.
// 
//     	@author Manuel Diaz <Manuel.Diaz.Gomez@cern.ch>


class ITriggerTranslatorTool : virtual public IAlgTool
                         
{
 public:

   // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITriggerTranslatorTool; }
  
  //Virtual destructor
  virtual ~ITriggerTranslatorTool() {}
  
  // book & fill (pure virtual)
  virtual const std::vector<std::string> translate(const std::string&) const = 0;
};
#endif
