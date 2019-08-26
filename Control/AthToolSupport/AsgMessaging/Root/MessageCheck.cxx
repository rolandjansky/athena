/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgMessaging/MessageCheck.h>

#include <stdexcept>

//
// method implementations
//

namespace asg
{
  ANA_MSG_SOURCE (msgUserCode, "UserCode")

  namespace detail
  {
    void throw_check_fail (const std::string& str)
    {
      throw std::runtime_error (str);
    }
  }
}
