/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class LWHistTraceUtils                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2010                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifdef LW_DEBUG_HEAVY_USERS//Don't produce code for these methods if not needed


#include "LWHistTraceUtils.h"
#include <execinfo.h>
#include <stdlib.h>

namespace LWHistTraceUtils {

  std::pair<void*,void*> getAdresses()
  {
    void *array[3];
    size_t size = backtrace (array, 3);
    if (size<3)
      return std::pair<void*,void*>(0,0);
    return std::pair<void*,void*>(array[1],array[2]);
  }

  char * getSymbol(void* address)
  {
    if (!address)
      return 0;
    void *array[1];
    array[0]=address;
    char **strings = backtrace_symbols (array,1);
    return strings[0];
  }
}

#endif
