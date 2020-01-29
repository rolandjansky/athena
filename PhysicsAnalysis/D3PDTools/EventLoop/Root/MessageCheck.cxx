/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



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
