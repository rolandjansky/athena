/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDataQualityToolBase_H
#define IDataQualityToolBase_H

#include "GaudiKernel/IAlgTool.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IDataQualityToolBase("IDataQualityToolBase", 1 , 0); 


//	The interface implemented by concrete Monitoring tools.
//      Concrete tools, derived from the MonitorToolBase base abstract
//	class are controlled via this interface.
// 
//     	@author Manuel Diaz <Manuel.Diaz.Gomez@cern.ch>


class IDataQualityToolBase : virtual public IAlgTool
                         
{
 public:

   // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IDataQualityToolBase; }

  //Virtual destructor
  virtual ~IDataQualityToolBase() {}
  
  // book & fill (pure virtual)
  virtual StatusCode bookHists()   = 0;
  virtual StatusCode fillHists()   = 0;
  virtual StatusCode finalHists()  = 0;
  virtual StatusCode checkHists(bool fromFinalize) = 0;
 
};
#endif
