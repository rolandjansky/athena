/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IMEMORYMONITORSVC_H
#define ATHENAKERNEL_IMEMORYMONITORSVC_H 1

///////////////////////////////////////////////////////////////////////////
//
// IMemoryMonitorSvc.h
//
// Abstract interface for
//
// 
//
///////////////////////////////////////////////////////////////////////////


#include "GaudiKernel/IService.h"

#include <string>
/**
 * @class IMemoryMonitorSvc
 * @brief Abstract interface for MemoryMonitorSvc. FIXME
 * @author Charles Leggett
 */
class IMemoryMonitorSvc : virtual public IService
{ 

 public: 

  static const InterfaceID& interfaceID();
  virtual void addItem( const std::string& type, size_t size ) = 0;

};

inline const InterfaceID& IMemoryMonitorSvc::interfaceID() {
    static const InterfaceID m_IID("IMemoryMonitorSvc", 1, 0);
    return m_IID;
}

#endif
