/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/ISelectionAccessor.h>

#include <AsgTools/StatusCode.h>
#include <SelectionHelpers/SelectionAccessorBits.h>
#include <SelectionHelpers/SelectionAccessorChar.h>
#include <SelectionHelpers/SelectionAccessorInvert.h>
#include <SelectionHelpers/SelectionAccessorList.h>
#include <SelectionHelpers/SelectionAccessorNull.h>
#include <SelectionHelpers/SelectionExprParser.h>
#include <exception>
#include <unordered_map>

//
// method implementations
//

namespace CP
{
  namespace
  {
    std::vector<std::string>
    splitString (const std::string& input, const std::string& separator)
    {
      std::vector<std::string> result;
      std::string::size_type start = 0, split = 0;
      while ((split = input.find (separator, start)) != std::string::npos)
      {
        result.emplace_back (input.substr (start, split - start));
        start = split + separator.size();
      }
      result.emplace_back (input.substr (start));
      return result;
    }

  }
  
    StatusCode 
    makeSelectionAccessor(const std::string& expr, 
                                      std::unique_ptr<ISelectionAccessor>& accessor,
                                      bool defaultToChar)
    {
      using namespace msgSelectionHelpers;

      if (expr.empty())
      {
        accessor = std::make_unique<SelectionAccessorNull> (true);
        return StatusCode::SUCCESS;
      }

      try {
        SelectionExprParser parser(expr, defaultToChar);
        ANA_CHECK(parser.build(accessor));
      } catch (const std::exception& e) {
        ANA_MSG_FATAL("Failure to parse expression: '" << expr << "': " << e.what());
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    }


  StatusCode
  makeSelectionAccessorVar (const std::string& name,
                         std::unique_ptr<ISelectionAccessor>& accessor,
                         bool defaultToChar)
  {
    using namespace msgSelectionHelpers;

    std::string var;
    bool asChar = false;
    bool asBits = false;
    bool invert = false;

    for (const std::string& option : splitString (name, ","))
    {
      if (var.empty())
      {
        // this is a bit of a hack, it will pick up the first
        // component as the decoration name
        var = option;
      } else if (option == "as_char")
      {
        // using ",as_char" as a postfix to indicate char decorations
        // (and ",as_bits" as a postfix to indicate bitset
        // decorations).  I chose that suffix as it should make it
        // easier to read in configuration files and allows for future
        // extensions if needed.
        asChar = true;
      } else if (option == "as_bits")
      {
        asBits = true;
      } else if (option == "invert")
      {
        invert = true;
      } else
      {
        ANA_MSG_ERROR ("invalid option " << option << "for selection decoration");
        return StatusCode::FAILURE;
      }
    }

    if (asChar && asBits)
    {
      ANA_MSG_ERROR ("can't specify both 'as_bits' and 'as_char' for the same selection decoration, pick one!!!");
      return StatusCode::FAILURE;
    }

    if (!asChar && !asBits)
    {
      if (defaultToChar)
        asChar = true;
      else
        asBits = true;
    }

    if (asChar)
      accessor = std::make_unique<SelectionAccessorChar> (var);
    else
      accessor = std::make_unique<SelectionAccessorBits> (var);

    if (invert)
    {
      accessor = std::make_unique<SelectionAccessorInvert>
        (std::move (accessor));
    }

    return StatusCode::SUCCESS;
  }
}
