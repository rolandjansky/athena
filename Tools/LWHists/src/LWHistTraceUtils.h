/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHistTraceUtils                    //
//                                                            //
//  Description: Utilities for figuring out who called a      //
//               method                                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2010                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHISTTRACEUTILS_H
#define LWHISTTRACEUTILS_H

#ifdef LW_DEBUG_HEAVY_USERS//Don't produce code for these methods if not needed

#include <utility>

namespace LWHistTraceUtils {

  std::pair<void*,void*> getAdresses();//Returns (me,mycaller)
  char * getSymbol(void* address);//Must call "free" on the returned pointer (or leak)

}

#endif

#endif
