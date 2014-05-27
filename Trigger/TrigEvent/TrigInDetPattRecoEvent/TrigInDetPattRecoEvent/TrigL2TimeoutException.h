/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_TIMEOUT_EXCEPTION_H__
#define __TRIG_L2_TIMEOUT_EXCEPTION_H__

class TrigL2TimeoutException {
 public:
    TrigL2TimeoutException(int s) : m_stage(s) {};
      TrigL2TimeoutException() : m_stage(0) {};
    virtual ~TrigL2TimeoutException() {};
    int m_stage;
};

#endif
