/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_PROMPTUTILS_H
#define PROMPT_PROMPTUTILS_H

/**********************************************************************************
 * @Package: JetTagNonPromptLepton
 * @Class  : PromptUtils
 * @Author : Rustem Ospanov
 * @Author : Rhys Roberts
 *
 * @Brief  :
 * 
 *  Helper functions
 *  
 **********************************************************************************/

// ROOT
#include "TStopwatch.h"

namespace Prompt
{
  std::string PrintResetStopWatch(TStopwatch &watch);

  class TimerScopeHelper
  {
  public:
    
    explicit TimerScopeHelper(TStopwatch &timer) 
      :fTimer(timer) { fTimer.Start(false); }
    ~TimerScopeHelper() { fTimer.Stop(); }
    
  private:
      
    TStopwatch &fTimer;
  };
}

#endif //PROMPT_PROMPTUTILS_H
