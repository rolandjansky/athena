/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENAEVTLOOPPRESELECTTOOL_H
#define ATHENAKERNEL_IATHENAEVTLOOPPRESELECTTOOL_H

/** @file IAthenaEvtLoopPreSelectTool.h
 *  @brief This file contains the class definition for the IAthenaEvtLoopPreSelectTool class.
 *  @author Marjorie Shapiro <mdshapiro@lbl.gov>
 **/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IAthenaEvtLoopPreSelectTool 
 *  @brief This class provides the interface for AthenaEvtLoopPreSelectTool classes used by AthenaEventLoopMgr
 **/

class EventInfo;

class IAthenaEvtLoopPreSelectTool : virtual public IAlgTool {

public:    

  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IAthenaEvtLoopPreSelectTool ("IAthenaEvtLoopPreSelectTool", 1, 0 );
    return IID_IAthenaEvtLoopPreSelectTool;
  }
  
  
  /// Initialize AlgTool
  virtual StatusCode initialize() = 0;
  /// called for each event to decide if the event should be passed to the EventSelector
  virtual bool passEvent(const EventInfo* pEvent) = 0;
  /// Finalize AlgTool
  virtual StatusCode finalize() = 0;
};

#endif
