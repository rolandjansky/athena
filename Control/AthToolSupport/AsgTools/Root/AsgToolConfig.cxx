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



  ::StatusCode AsgToolConfig ::
  makeTool (std::unique_ptr<AsgTool>& tool) const
  {
    using namespace msgComponentConfig;

    ANA_CHECK (makeComponentExpert (tool, "new %1% (\"%2%\")", true));
    ANA_CHECK (tool->initialize());

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }
}
