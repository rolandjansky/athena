/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_EXCEPTIONHANDLER_H
#define TRIGINTERFACES_EXCEPTIONHANDLER_H

#include "TrigSteeringEvent/Enums.h"

class MsgStream;

namespace HLT {

  /**
   * Trigger exception handler
   *
   * @param log Pointer to MsgStream (be silent if 0)
   *
   * Use this exception handler in core trigger code rather than coding individual
   * catch statements yourself. This will ensure uniform treatment of exceptions and
   * and allow easy changes to the exception policies. The handler will either return
   * and convert the exception into an HLT::ErrorCode, or re-throw the exception depending
   * on the severity. 
   *
   * Example usage: \code
     try {
       // ... something that might throw
       return HLT::OK;
     } catch (...) {
       return trigExceptionHandler();
     }     
   * \endcode
   */
  ErrorCode trigExceptionHandler(MsgStream* log = 0);
}

#endif
