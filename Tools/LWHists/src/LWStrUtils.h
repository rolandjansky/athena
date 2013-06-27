/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWStrUtils                          //
//                                                            //
//  Description: Utilities for handling c-style char strings, //
//               allocating with LWPools.                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2009                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWSTRUTILS_H
#define LWSTRUTILS_H

#include "LWPools.h"
#include <cassert>
#include <cstring>
#include <iostream>

class LWStrUtils {
public:
  static const unsigned maxStringLength = 1023;//We don't want any crazy long strings...
  static uint16_t strLength(const char*c);
  static void setStringFromInput(const char* input, char*& target );
  static void releaseString(char*& c);
  static bool stringsEqual(const char*c1,const char*c2);

private:
  LWStrUtils();
  ~LWStrUtils();
};

///////////////
//  Inlines  //
///////////////

inline uint16_t LWStrUtils::strLength(const char*c) {
  if (!c)
    return 0;
  uint16_t i(0);
  for (;i<maxStringLength;++i) {
    if (c[i]=='\0')
      return i;
  }
  if (c[maxStringLength]!='\0') {
    std::cout<<"LWHist::WARNING: String longer than "<<maxStringLength<<". Truncating."<<std::endl;
    std::cout<<"LWHist::WARNING: String was:"<<c<<std::endl;
  }
  return maxStringLength;
}

inline void LWStrUtils::setStringFromInput(const char* input, char*& target ) {
  if (target)
    releaseString(target);
  const uint16_t l = strLength(input);

  assert((target-input<0?-(target-input):target-input)>l+1);
  target = LWPools::acquire(l+1);

  memcpy(target,input,l);
  target[l]='\0';
}

inline void LWStrUtils::releaseString(char*& c)
{
  if (c) {
    const uint16_t prevL = strLength(c);
    LWPools::release(c,prevL+1);
    c = 0;
  }
}

inline bool LWStrUtils::stringsEqual(const char*c1,const char*c2) {
  return c1==c2 || (c1&&c2&&strncmp(c1,c2,maxStringLength)==0);
}


#endif
