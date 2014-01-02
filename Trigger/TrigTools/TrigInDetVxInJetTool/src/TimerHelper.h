/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TIMERHELPER_H_
#define _TIMERHELPER_H_
#include "TrigTimeAlgs/TrigTimer.h"


class TimerHelper
{
public:

  TimerHelper( TrigTimer* timer )
    : m_timer( timer )
  {
    if ( m_timer ) m_timer->start();
  }

  ~TimerHelper()
  {
    if ( m_timer ) m_timer->stop();
  }

private:

  TrigTimer* m_timer;
};

#endif /* _TIMERHELPER_H_ */
