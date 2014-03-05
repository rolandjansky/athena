// MsgLevel.h

#ifndef MsgLevel_H
#define MsgLevel_H

// David Adams
// January 2014
//
// Defines the message levels and provides a function to convert
// these to strings.

#include <string>
#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/IMessageSvc.h"
#endif

#undef ERROR

namespace MSG {

#ifndef ASGTOOL_ATHENA
enum Level {
  NIL = 0,
  VERBOSE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL,
  ALWAYS,
  NUM_LEVELS
};
#endif

std::string name(Level lvl);

}  // end namespace MSG

#endif
