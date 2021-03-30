/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IHYBRIDPROCESSORHELPER_H
#define ATHENAKERNEL_IHYBRIDPROCESSORHELPER_H

/**
 * @file  IHybridProcessorHelper.h
 * @class IHybridProcessorHelper
 * @brief Helper interface for implementing hybrid MP+MT.
          Used by the Hybrid Shared Event Queue Consumer MP tool
 **/

class IHybridProcessorHelper {
 public: 
  virtual void resetAppReturnCode() = 0;
  virtual void setCurrentEventNum(int num) = 0;
  virtual bool terminateLoop() = 0;
  virtual int drainScheduler(int& finishedEvts, bool report) = 0;
};

#endif
