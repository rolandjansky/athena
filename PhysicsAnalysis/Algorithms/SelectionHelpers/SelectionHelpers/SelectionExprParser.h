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

namespace DetailSelectionExprParser {
class Separator {
 public:
  bool operator()(std::string::const_iterator &next,
                  const std::string::const_iterator &end,
                  std::string &tok) const;

  void reset() {}
};

using Tokenizer = boost::tokenizer<Separator>;

class Lexer {
 public:
  Lexer(const std::string &s);
  Lexer(const Lexer &) = delete;
  Lexer(Lexer &&) = default;

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

  struct Symbol {
    Type type;
    std::string value;
  };

  Symbol nextSymbol();

 private:
  std::string m_string;
  Tokenizer m_tokenizer;
  Tokenizer::iterator m_iterator;
};
}  // namespace DetailSelectionExprParser

class SelectionExprParser {
 public:
  SelectionExprParser(DetailSelectionExprParser::Lexer lexer,
                      bool defaultToChar = false);
  StatusCode build(std::unique_ptr<ISelectionAccessor> &accessor);

 private:
  StatusCode expression(std::unique_ptr<ISelectionAccessor> &root);
  StatusCode term(std::unique_ptr<ISelectionAccessor> &root);
  StatusCode factor(std::unique_ptr<ISelectionAccessor> &root);

  DetailSelectionExprParser::Lexer m_lexer;
  DetailSelectionExprParser::Lexer::Symbol m_symbol;
  bool m_defaultToChar;
};

}  // namespace CP

#endif
