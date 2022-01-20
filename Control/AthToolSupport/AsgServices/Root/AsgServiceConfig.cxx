/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgServices/AsgServiceConfig.h>

#include <AsgServices/AsgService.h>
#include <AsgTools/MessageCheckAsgTools.h>

//
// method implementations
//

namespace asg
{
  AsgServiceConfig ::
  AsgServiceConfig (const std::string& val_typeAndName)
    : AsgComponentConfig (val_typeAndName)
  {}



  AsgServiceConfig ::
  AsgServiceConfig (const AsgComponentConfig& val_config)
    : AsgComponentConfig (val_config)
  {}
}
