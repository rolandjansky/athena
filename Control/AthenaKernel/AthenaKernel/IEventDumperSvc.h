/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IEVENTDUMPERSVC_H
#define ATHENAKERNEL_IEVENTDUMPERSVC_H 1

///////////////////////////////////////////////////////////////////////////
//
// IEventDumperSvc.h
//
// 
//
// author: 
//
///////////////////////////////////////////////////////////////////////////


#include "GaudiKernel/IService.h"


/** @class IEventDumperSvc
 * @brief Abstract interface for EventDumperSvc. Just a placeholder for now
 *   @author Charles Leggett
 */  

class IEventDumperSvc : virtual public IService
{ 

 public: 

  static const InterfaceID& interfaceID();

};

inline const InterfaceID& IEventDumperSvc::interfaceID() {
    static const InterfaceID m_IID("IEventDumperSvc", 1, 0);
    return m_IID;
}

#endif
