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
                         std::unique_ptr<ISelectionAccessor>& accessor,
                         bool defaultToChar)
  {
    using namespace msgSelectionHelpers;

    if (name.empty())
    {
      ANA_MSG_ERROR ("provided empty name for selection decoration");
      return StatusCode::FAILURE;
    }

    bool asChar = false;
    bool asBits = false;
    std::string var;

    // using ",as_char" as a postfix to indicate char decorations (as
    // ",as_bits" as a postfix to indicate bitset decorations).  I
    // chose that suffix as it should make it easier to read in
    // configuration files and allows for future extensions if needed.
    const auto split = name.find (',');
    if (split != std::string::npos)
      var = name.substr (0, split);
    else
      var = name;

    if (split != std::string::npos)
    {
      if (name.substr (split + 1) == "as_char")
      {
        asChar = true;
      } else if (name.substr (split + 1) != "as_bits")
      {
        asBits = true;
      } else
      {
        ANA_MSG_ERROR ("invalid suffix " << name.substr (split + 1) << "for selection decoration");
        return StatusCode::FAILURE;
      }
    }

    if (!asChar && !asBits)
    {
      if (defaultToChar)
        asChar = true;
      else
        asBits = true;
    }

    if (asChar)
      accessor = std::make_unique<SelectionAccessorOR> (var);
    else
      accessor = std::make_unique<SelectionAccessorRegular> (var);
    return StatusCode::SUCCESS;
  }
}
