/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMONITORTOOLBASE_H
#define IMONITORTOOLBASE_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMonitorToolBase("IMonitorToolBase", 1 , 0); 


//	The interface implemented by concrete Monitoring tools.
//      Concrete tools, derived from the MonitorToolBase base abstract
//	class are controlled via this interface.
// 
//     	@author Manuel Diaz <Manuel.Diaz.Gomez@cern.ch>


class IMonitorToolBase : virtual public IAlgTool
                         
{
 public:

   // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMonitorToolBase; }
  
  virtual StatusCode setupOutputStreams(std::vector<std::string> Mapping
					= std::vector<std::string>()     ) = 0;
  
  //Virtual destructor
  virtual ~IMonitorToolBase() {}
  
  // book & fill (pure virtual)
  virtual StatusCode bookHists() = 0;
  virtual StatusCode fillHists() = 0;
  virtual StatusCode finalHists() = 0;
  virtual StatusCode runStat() = 0;
  virtual StatusCode checkHists(bool fromFinalize) = 0;
  virtual bool       preSelector() = 0;
  virtual StatusCode convertLWHists() = 0; 
};
#endif
