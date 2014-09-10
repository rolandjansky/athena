// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATEBINDINGS_SGPYMSG_H
#define STOREGATEBINDINGS_SGPYMSG_H 1

#include <iostream>

// enable debugging printouts !!! INTERNAL!!! DO NOT USE !!!
//#define _SGPY_DO_DEBUG 1
#ifdef _SGPY_DO_DEBUG
# define _SGPY_MSG(x)                           \
  std::cerr                                     \
  << "_SGPY_DEBUGGING__:"                       \
  << __LINE__ <<": "                            \
  << x                                          \
  << "\n"
#else
# define _SGPY_MSG(x)                           \
  do {} while (0)
#endif
// -----------

#endif // !STOREGATEBINDINGS_SGPYMSG_H

