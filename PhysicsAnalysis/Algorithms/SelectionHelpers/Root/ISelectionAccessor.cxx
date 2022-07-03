/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AsgMessaging/StatusCode.h>
#include <SelectionHelpers/SelectionReadAccessorBits.h>
#include <SelectionHelpers/SelectionReadAccessorChar.h>
#include <SelectionHelpers/SelectionWriteAccessorBits.h>
#include <SelectionHelpers/SelectionWriteAccessorChar.h>
#include <SelectionHelpers/SelectionWriteAccessorSys.h>
#include <SelectionHelpers/SelectionAccessorList.h>
#include <SelectionHelpers/SelectionReadAccessorNull.h>
#include <SelectionHelpers/SelectionAccessorReadSys.h>
#include <SelectionHelpers/SelectionExprParser.h>
#include <SelectionHelpers/SelectionReadAccessorInvert.h>
#include <SelectionHelpers/SelectionWriteAccessorInvert.h>
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
  makeSelectionReadAccessor(const std::string& expr,
                            std::unique_ptr<ISelectionReadAccessor>& accessor,
                            bool defaultToChar)
  {
    using namespace msgSelectionHelpers;

    if (expr.empty())
    {
      accessor = std::make_unique<SelectionReadAccessorNull> (true);
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


  namespace
  {
    struct SplitData final
    {
      std::string var;
      bool asChar = false;
      bool asBits = false;
      bool invert = false;

      StatusCode fill (const std::string& name, bool defaultToChar)
      {
        using namespace msgSelectionHelpers;

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
        return StatusCode::SUCCESS;
      }
    };
  }


  StatusCode
  makeSelectionReadAccessorVar (const std::string& name,
                                std::unique_ptr<ISelectionReadAccessor>& accessor,
                                bool defaultToChar)
  {
    using namespace msgSelectionHelpers;

    if (name.find ("%SYS%") != std::string::npos)
    {
      accessor = std::make_unique<SelectionAccessorReadSys>(name);
      return StatusCode::SUCCESS;
    }

    SplitData splitData;
    ANA_CHECK (splitData.fill (name, defaultToChar));

    if (splitData.asChar)
      accessor = std::make_unique<SelectionReadAccessorChar> (splitData.var);
    else
      accessor = std::make_unique<SelectionReadAccessorBits> (splitData.var);

    if (splitData.invert)
    {
      accessor = std::make_unique<SelectionReadAccessorInvert>
        (std::move (accessor));
    }

    return StatusCode::SUCCESS;
  }


  StatusCode
  makeSelectionWriteAccessor (const std::string& name,
                                 std::unique_ptr<ISelectionWriteAccessor>& accessor,
                                 bool defaultToChar)
  {
    using namespace msgSelectionHelpers;

    if (name.find ("%SYS%") != std::string::npos)
    {
      accessor = std::make_unique<SelectionWriteAccessorSys>(name);
      return StatusCode::SUCCESS;
    }

    SplitData splitData;
    ANA_CHECK (splitData.fill (name, defaultToChar));

    if (splitData.asChar)
      accessor = std::make_unique<SelectionWriteAccessorChar> (splitData.var);
    else
      accessor = std::make_unique<SelectionWriteAccessorBits> (splitData.var);

    if (splitData.invert)
    {
      accessor = std::make_unique<SelectionWriteAccessorInvert>
        (std::move (accessor));
    }

    return StatusCode::SUCCESS;
  }
}
