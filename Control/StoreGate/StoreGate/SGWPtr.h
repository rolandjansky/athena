/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SGWPtr.h
 *  return type of StoreGateSvc::create. 
 *  Currently a plain pointer, may become a handle informing the scheduler
 *  when we are done "writing"
 */
#ifndef STOREGATE_SGWPTR_H
#define STOREGATE_SGWPTR_H
#include <boost/config.hpp>
namespace SG {
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
  template<class T> using WPtr = T*;
#endif
}
#endif // STOREGATE_SGWPTR_H
