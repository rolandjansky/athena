/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTION_HELPERS__SELECTION_EXPR_PARSER_H
#define SELECTION_HELPERS__SELECTION_EXPR_PARSER_H

#include <boost/tokenizer.hpp>
#include <functional>
#include <memory>
#include <string>

#include "SelectionHelpers/ISelectionAccessor.h"
#include "SelectionHelpers/SelectionHelpers.h"

namespace CP {

// Private detail namespace. This is not strictly private to enable testing
namespace DetailSelectionExprParser {

/// Separator to be used in a @c boost::tokenizer
class Separator {
 public:
  /// @brief Extracts the subsequent token from the input string iterator
  bool operator()(std::string::const_iterator &next,
                  const std::string::const_iterator &end,
                  std::string &tok) const;

  /// @brief Optional state reset method, does nothing.
  void reset() {}
};

// Typedef over boost's tokenizer using the above Separator
typedef boost::tokenizer<Separator> Tokenizer;

/// Lexer which turns a token stream into a stream of unambigous symbols to be
/// used by a parser
class Lexer {
 public:
  /// @brief Constructor from a strig
  Lexer(const std::string &s);

  /// Disable copying of this class
  Lexer(const Lexer &) = delete;
  /// Default move-constructor behaviour
  Lexer(Lexer &&) = default;

  /// Enum over the possible symbols that can be extracted from the token
  /// stream.
  enum Type {
    AND = 0,
    OR,
    LEFT,
    RIGHT,
    NOT,
    TRUE_LITERAL,
    FALSE_LITERAL,
    VAR,
    END
  };

  /// Struct grouping together the type and original string representation
  /// of a symbol.
  struct Symbol {
    Type type;
    std::string value;
  };

  /// Generate a new symbol from the token sequence
  Symbol nextSymbol();

 private:
  std::string m_string;
  Tokenizer m_tokenizer;
  Tokenizer::iterator m_iterator;
};
}  // namespace DetailSelectionExprParser

/// Public interface for the expression parsing facility.
class SelectionExprParser {
 public:
  /// Constructor for the parser which accepts a @c Lecer
  /// @param lexer The lexer to use for parsing
  /// @note @c Lexer can be auto-constructed from a string, so you can pass one directly.
  /// @param defaultToChar Assume "as_char" as default encoding
  SelectionExprParser(DetailSelectionExprParser::Lexer lexer,
                      bool defaultToChar = false);

  /// Triggers the actual parsing of the expression
  /// @param [out] accessor Unique pointer the resulting accessor will be written to.
  /// @return StatusCode noting whether the operation succeeded.
  StatusCode build(std::unique_ptr<ISelectionAccessor> &accessor);

 private:
  // Construct a binary OR
  StatusCode expression(std::unique_ptr<ISelectionAccessor> &root);
  // Construct an AND, attempts to group all ANDs it can see into a list 
  StatusCode term(std::unique_ptr<ISelectionAccessor> &root);
  // Handle other constructs, potentially more ORs or ANDs.
  StatusCode factor(std::unique_ptr<ISelectionAccessor> &root);

  // The lexer to generate symbols from.
  DetailSelectionExprParser::Lexer m_lexer;
  // The last extracted symbol
  DetailSelectionExprParser::Lexer::Symbol m_symbol;
  // Stores constructor parameter to be used in calls to makeSelectionAccessorVar.
  bool m_defaultToChar;
};

}  // namespace CP

#endif
