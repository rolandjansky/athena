/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ELECTRONPHOTONSELECTORTOOLS_FAKEMSGSTREAMANDSC__
#define __ELECTRONPHOTONSELECTORTOOLS_FAKEMSGSTREAMANDSC__
/**
   @brief  Macros to "fake" the MSG stream, just outputting to stderr. A step back, of course. Also fake sc

   @author Jovan Mitrevski
   @date   Dec. 2011
*/

#include <iostream>

#define FAKE_MSG_FATAL(x)               \
  std::cerr << "FATAL: " << x << std::endl;

#define FAKE_MSG_ERROR(x)               \
  std::cerr << "ERROR: " << x << std::endl;

#define FAKE_MSG_WARNING(x)               \
  std::cerr << "WARNING: " << x << std::endl;

#define FAKE_MSG_INFO(x)               \
  std::cout << "INFO: " << x << std::endl << std::flush;

#define FAKE_MSG_DEBUG(x)               \
  std::cout << "DEBUG: " << x << std::endl << std::flush;

namespace FkStatusCode {
  enum {
    FAILURE = 0,
    SUCCESS = 1,
    RECOVERABLE = 2
  };
}

typedef int FakeStatusCode;

#endif
