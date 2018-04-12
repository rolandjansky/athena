//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/AnaToolHandle.h>

//
// main program
//

int main (int argc, char *argv [])
{
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  if (argc < 2)
  {
    ANA_MSG_ERROR ("missing argument: [type]");
    return -1;
  }

  asg::AnaToolHandle<asg::IAsgTool> tool (std::string (argv[1]) + "/tool");
  for (int iter = 2; iter != argc; ++ iter)
  {
    const std::string arg = argv[iter];
    const auto split = arg.find ("=");
    if (split == std::string::npos)
    {
      ANA_MSG_ERROR ("argument doesn't match name=value syntax: \"" << arg << "\"");
      return -1;
    }
    ANA_CHECK (tool.setProperty (arg.substr (0, split), arg.substr (split + 1)));
  }
  ANA_CHECK (tool.initialize());
  ANA_MSG_INFO ("successfully initialized tool");
  return 0;
}
