/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

#include <EventLoop/MessageCheck.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  ANA_MSG_SOURCE (msgEventLoop, "EventLoop")

  namespace Detail
  {
    void report_exception ()
    {
      using namespace msgEventLoop;
      try
      {
        throw;
      } catch (std::exception& e)
      {
        ANA_MSG_ERROR ("caught exception: " << e.what());
      } catch (std::string& str)
      {
        ANA_MSG_ERROR ("caught exception string: " << str);
      } catch (const char *str)
      {
        ANA_MSG_ERROR ("caught exception string: " << str);
      } catch (...)
      {
        ANA_MSG_ERROR ("caught unknown exception");
      }
    }
  }
}
