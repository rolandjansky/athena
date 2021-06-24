/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/AsgToolConfig.h>

#include <AsgTools/AsgTool.h>
#include <AsgTools/MessageCheckAsgTools.h>

//
// method implementations
//

namespace asg
{
  AsgToolConfig ::
  AsgToolConfig (const std::string& val_typeAndName)
    : AsgComponentConfig (val_typeAndName)
  {}



  AsgToolConfig ::
  AsgToolConfig (const AsgComponentConfig& val_config)
    : AsgComponentConfig (val_config)
  {}
}
