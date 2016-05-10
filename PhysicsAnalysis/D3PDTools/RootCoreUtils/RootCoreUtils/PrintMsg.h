#ifndef ROOT_CORE_UTILS__PRINT_MSG_H
#define ROOT_CORE_UTILS__PRINT_MSG_H

//          Copyright Nils Krumnack 2011 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines macros for reporting errors.  The interface in
/// this module is indended for experts only.  This module is
/// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

#include <string>
#include <RootCoreUtils/MessageType.h>

class TString;

namespace RCU
{
  void send_message (const char *package, const char *file, unsigned line,
		     MessageType type, const char *msg);
  void send_message (const char *package, const char *file, unsigned line,
		     MessageType type, const std::string& msg);
  void send_message (const char *package, const char *file, unsigned line,
		     MessageType type, const TString& msg);
  [[noreturn]] void
  send_message_abort (const char *package, const char *file, unsigned line,
		      MessageType type, const char *msg);
  [[noreturn]] void
  send_message_abort (const char *package, const char *file, unsigned line,
		      MessageType type, const std::string& msg);
  [[noreturn]] void
  send_message_abort (const char *package, const char *file, unsigned line,
		      MessageType type, const TString& msg);
}

#ifndef ROOTCORE_PACKAGE
#define ROOTCORE_PACKAGE 0
#endif

#define RCU_PRINT_MSG(message)						\
  ::RCU::send_message (ROOTCORE_PACKAGE, __FILE__, __LINE__, ::RCU::MESSAGE_REGULAR, (message));

#define RCU_WARN_MSG(message)						\
  ::RCU::send_message (ROOTCORE_PACKAGE, __FILE__, __LINE__, ::RCU::MESSAGE_WARNING, (message));

#define RCU_ERROR_MSG(message)						\
  ::RCU::send_message (ROOTCORE_PACKAGE, __FILE__, __LINE__, ::RCU::MESSAGE_ERROR, (message));

#define RCU_THROW_MSG(message)						\
  ::RCU::send_message_abort (ROOTCORE_PACKAGE, __FILE__, __LINE__, ::RCU::MESSAGE_EXCEPTION, (message));

#endif
