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
#include <SelectionHelpers/SelectionAccessorExpr.h>
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


    StatusCode
    makeSelectionAccessorList (const std::string& name,
                               std::unique_ptr<ISelectionAccessor>& accessor,
                               bool defaultToChar)
    {
      using namespace msgSelectionHelpers;

      std::vector<std::string> subnames = splitString (name, "&&");

      std::vector<std::unique_ptr<ISelectionAccessor> > list;
      for (const std::string& subname : subnames)
      {
        if (subname.empty())
        {
          ANA_MSG_ERROR ("provided empty selection decoration in decoration list: " << name);
          return StatusCode::FAILURE;
        }

        std::unique_ptr<ISelectionAccessor> subaccessor;
        ANA_CHECK (makeSelectionAccessor (subname, subaccessor, defaultToChar));
        list.emplace_back (std::move (subaccessor));
      }
      accessor = std::make_unique<SelectionAccessorList> (std::move (list));
      return StatusCode::SUCCESS;
    }

    StatusCode 
    makeSelectionAccessorFromExpression(const std::string& name, 
                                      std::unique_ptr<ISelectionAccessor>& accessor,
                                      bool defaultToChar)
    {
      using namespace msgSelectionHelpers;

      try {
        SelectionExprParser parser(name);
        std::unique_ptr<SelectionExprParser::BooleanExpression> ex =
            parser.build();
        ANA_MSG_VERBOSE("Expression: " << ex->toString());

        std::unordered_map<std::string, std::unique_ptr<ISelectionAccessor>>
            accessors;
        ex->visit([&](const SelectionExprParser::BooleanExpression& subex) {
          const SelectionExprParser::Variable* var = dynamic_cast<const SelectionExprParser::Variable*>(&subex);
          if (var == nullptr) {
            return;
          }

          accessors[var->name()] = nullptr;
        });

        for (const auto& it : accessors) {
          ANA_CHECK(makeSelectionAccessor(it.first, accessors[it.first], defaultToChar));
        }

        accessor = std::make_unique<SelectionAccessorExpr>(std::move(ex), std::move(accessors));

      } catch (...) {
        ANA_MSG_FATAL("Failure to parse expression: '" << name << "'");
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    }

  }



  StatusCode
  makeSelectionAccessor (const std::string& name,
                         std::unique_ptr<ISelectionAccessor>& accessor,
                         bool defaultToChar)
  {
    using namespace msgSelectionHelpers;

    if (name.empty())
    {
      accessor = std::make_unique<SelectionAccessorNull> ();
      return StatusCode::SUCCESS;
    }

    // treat decoration lists separately

    // if && and NO OTHER operators are found: use SelectionAccessorList
    if (name.find("&&") != std::string::npos &&
        name.find("||") == std::string::npos &&
        name.find("(") == std::string::npos &&
        name.find(")") == std::string::npos &&
        name.find("!") == std::string::npos) {
      return makeSelectionAccessorList(name, accessor, defaultToChar);
    }

    // if ANY OTHER operator is present: ise SelectionAccessorExpr
    if (name.find("||") != std::string::npos ||
        name.find("(") != std::string::npos ||
        name.find(")") != std::string::npos ||
        name.find("!") != std::string::npos) {
      return makeSelectionAccessorFromExpression(name, accessor, defaultToChar);
    }

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
