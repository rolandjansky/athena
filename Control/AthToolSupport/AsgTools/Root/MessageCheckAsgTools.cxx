/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgMessaging/MessageCheck.h>
#include <AsgTools/MessageCheckAsgTools.h>

//
// method implementations
//

namespace asg
{
  ANA_MSG_SOURCE (msgComponentConfig, "AsgTools.ComponentConfig")
  ANA_MSG_SOURCE (msgProperty, "AsgTools.Property")
  ANA_MSG_SOURCE (msgToolHandle, "AsgTools.AnaToolHandle")
  ANA_MSG_SOURCE (msgToolStore, "AsgTools.ToolStore")
}

// Ensure that TLS defined in this library actually gets used.
// Avoids a potential slowdown in accessing TLS seen in simualation.
// See ATLASSIM-4932.
namespace {
[[maybe_unused]]
thread_local int tlsDum;
[[maybe_unused]]
const int& msgDumRef = tlsDum;
}

