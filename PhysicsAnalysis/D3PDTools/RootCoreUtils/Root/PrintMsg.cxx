//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/PrintMsg.h>

#include <TString.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/Message.h>
#include <cstdlib>

//
// method implementations
//

namespace RCU
{
  void send_message (const char *package, const char *file, unsigned line,
		     MessageType type, const char *msg)
  {
    Message message;
    message.package = package;
    message.file = file;
    message.line = line;
    message.type = type;
    message.message = msg;
    message.send ();
  }



  void send_message (const char *package, const char *file, unsigned line,
		     MessageType type, const std::string& msg)
  {
    send_message (package, file, line, type, msg.c_str());
  }



  void send_message (const char *package, const char *file, unsigned line,
		     MessageType type, const TString& msg)
  {
    send_message (package, file, line, type, msg.Data());
  }



  void send_message_abort (const char *package, const char *file, unsigned line,
		     MessageType type, const char *msg)
  {
    Message message;
    message.package = package;
    message.file = file;
    message.line = line;
    message.type = type;
    message.message = msg;
    message.send ();
    RCU_ASSERT0 ("shouldn't get here");
    std::abort ();
  }



  void send_message_abort (const char *package, const char *file, unsigned line,
			   MessageType type, const std::string& msg)
  {
    send_message_abort (package, file, line, type, msg.c_str());
  }



  void send_message_abort (const char *package, const char *file, unsigned line,
			   MessageType type, const TString& msg)
  {
    send_message_abort (package, file, line, type, msg.Data());
  }
}
