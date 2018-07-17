/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/ISelectionAccessor.h>

#include <AsgTools/StatusCode.h>
#include <SelectionHelpers/SelectionAccessorRegular.h>
#include <SelectionHelpers/SelectionAccessorOR.h>
#include <exception>

//
// method implementations
//

namespace CP
{
  StatusCode
  makeSelectionAccessor (const std::string& name,
                         std::unique_ptr<ISelectionAccessor>& accessor)
  {
    using namespace msgSelectionHelpers;

    if (name.empty())
    {
      ANA_MSG_ERROR ("provided empty name for selection decoration");
      return StatusCode::FAILURE;
    }

    // using ",as_char" as a postfix to indicate char decorations.  I
    // chose that suffix as it should make it easier to read in
    // configuration files and allows for future extensions if needed.
    const auto split = name.find (',');
    if (split != std::string::npos)
    {
      if (name.substr (split + 1) != "as_char")
      {
        ANA_MSG_ERROR ("invalid suffix " << name.substr (split + 1) << "for selection decoration");
        return StatusCode::FAILURE;
      }
      accessor = std::make_unique<SelectionAccessorOR> (name.substr (0, split));
    } else
    {
      accessor = std::make_unique<SelectionAccessorRegular> (name);
    }
    return StatusCode::SUCCESS;
  }
}
