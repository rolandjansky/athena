//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/Message.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ExceptionMsg.h>
#include <RootCoreUtils/PrintMsg.h>

//
// method implementations
//

namespace RCU
{
  Message ::
  Message ()
    : package (0), file (0), line (0), type (MESSAGE_UNSPECIFIED),
      message (0)
  {
  }



  void Message ::
  send () const
  {
    MessageType mytype = type;
    if (mytype < 0 || mytype > MESSAGE_UNSPECIFIED)
      mytype = MESSAGE_UNSPECIFIED;

    std::ostringstream str;

    if (package != 0)
    {
      str << package << ":";
    }
    if (file != 0)
    {
      if (strncmp (file, "../", 3) == 0)
	str << (file+3) << ":";
      else
	str << file << ":";
    }
    if (line != 0)
      str << line << ":";

    if (mytype != MESSAGE_UNSPECIFIED)
    {
      static const char * const type_names[MESSAGE_UNSPECIFIED] =
	{"message", "warning", "error", "exception", "abort"};
      str << type_names[mytype] << ":";
    }

    if (!str.str().empty())
      str << " ";
    if (message != 0)
      str << message;
    else
      str << "(null)";

    const char *envname = 0;
    if (mytype == MESSAGE_ABORT)
      envname = "ROOTCOREUTILS_ABORT";
    else if (mytype == MESSAGE_EXCEPTION)
      envname = "ROOTCOREUTILS_EXCEPTION";
    if (envname)
    {
      const char *abort_type = getenv (envname);
      const MessageType def_type = MESSAGE_EXCEPTION;

      if (abort_type == 0)
      {
	mytype = def_type;
      } else if (strcmp (abort_type, "abort") == 0)
      {
	mytype = MESSAGE_ABORT;
      } else if (strcmp (abort_type, "exception") == 0)
      {
	mytype = MESSAGE_EXCEPTION;
      } else
      {
	mytype = def_type;

	RCU_WARN_MSG (std::string ("unknown value for ROOTCOREUTILS_ABORT ") + abort_type);
      }
    }

    std::cout << str.str() << std::endl;
    if (mytype == MESSAGE_EXCEPTION)
      throw ExceptionMsg (file, line, str.str());
    if (mytype == MESSAGE_ABORT)
      std::abort ();
  }
}
