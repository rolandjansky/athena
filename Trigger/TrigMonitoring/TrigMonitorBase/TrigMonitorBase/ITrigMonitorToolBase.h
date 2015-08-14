/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGMONITORTOOLBASE_H
#define ITRIGMONITORTOOLBASE_H

#include "GaudiKernel/IAlgTool.h"

//!< Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ITrigMonitorToolBase("ITrigMonitorToolBase", 1 , 0); 

/**
 * @brief The interface to be implemented by concrete Monitoring tools.
 *	
 * Concrete tools, derived from the TrigMonitorToolBase base abstract
 * class are controlled via this interface.
 * 
 * @author Manuel Diaz <Manuel.Diaz.Gomez@cern.ch>
 * @warning This interface is OUTDATED ans users are discurraged from using it. -- Tomasz Bold <Tomasz.Bold@cern.ch>
 */
template<typename T>
class ITrigMonitorToolBase : virtual public IAlgTool
                         
{
 public:
  static const InterfaceID& interfaceID() { return IID_ITrigMonitorToolBase; }   //!<  Retrieve interface ID

  virtual ~ITrigMonitorToolBase() {}
  

  virtual StatusCode bookHists() = 0;   //!< booking method
  virtual StatusCode fillHists(T) = 0;  //!< fill method (not that templation is happening here)
  //virtual StatusCode finalHists() = 0;

 
};

#endif // ITRIGMONITORTOOLBASE_H

