// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigTimerSvc.h,v 1.4 2007-07-06 16:26:08 tbold Exp $
// $Name: not supported by cvs2svn $
#ifndef TRIGTIMEALGS_ITRIGTIMERSVC_H
#define TRIGTIMEALGS_ITRIGTIMERSVC_H
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include <iostream>
#include <string>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID  IID_ITrigTimerSvc(1059,1,3);

class TrigTimer;

/**
 * @brief Interface class for acquire timers in trigger codes. 
 *
 * Even if the timers can be created as any other object if is 
 * good to use this service because it controls then deletion
 * and provides client tools willing to use this timers a single access entry point to all of them.
 *
 * Each timers is registered with the name and can with this name can be retrieved.
 * All timers can be iterated over easily with 2 iterator getters functions.
 */
class ITrigTimerSvc : virtual public IInterface  {  

public:

  //DeclareInterface(ITrigTimerSvc, 1, 3);
  static const InterfaceID& interfaceID() { return IID_ITrigTimerSvc; }
  
  /**
   * @brief the method to acquire timer
   * The timer name has to be given. As a result user gets pointer to the timer which can use.
   * The timer is still owned by the ITrigTimerSvc i.e. user shall not delete it.
   */
  virtual TrigTimer* addItem(const std::string& name)=0;

  /**
   * @brief iterator to the begin of the list of all timers owned by this service
   */
  virtual std::vector < TrigTimer* >::iterator begin( void )=0;

  /**
   * @brief iterator to the next to the last element of the list of all timers owned by this service
   */
  virtual std::vector < TrigTimer* >::iterator end( void )=0;

  /**
   * @brief pretty print the information about all timers owned by this service
   */
  virtual void print( void )=0;
};


#endif // TRIGTIMEALGS_ITRIGTIMERSVC_H

