/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionExprParser.h"
#include "SelectionHelpers/SelectionAccessorExprNot.h"
#include "SelectionHelpers/SelectionAccessorExprOr.h"
#include "SelectionHelpers/SelectionAccessorList.h"
#include "SelectionHelpers/SelectionAccessorNull.h"

#include <iostream>
#include <regex>
#include <string>

using CP::DetailSelectionExprParser::Lexer;

namespace CP {
using namespace msgSelectionHelpers;

namespace DetailSelectionExprParser {
bool Separator::operator()(std::string::const_iterator& next,
                           const std::string::const_iterator& end,
                           std::string& tok) const {
  if (next == end) {
    return false;
  }

  static const std::regex base_regex(
      "^( ?((?:[^|&()! ]+)|(?:&&)|(?:!)|(?:\\()|(?:\\))|(?:\\|\\|))).*");
  std::smatch m;

  if (std::regex_match(next, end, m, base_regex)) {
    tok = m[2].str();
    next += m[1].length();
  } else {
    std::smatch m2;
    static const std::regex space_re{"^ +$"};
    if (std::regex_match(next, end, m, space_re)) {
      // only spaces left, we're done
      return false;
    }
    throw std::runtime_error("Cannot tokenize: '" + std::string(next, end) +
                             "'");
  }
  return true;
}

Lexer::Lexer(const std::string& s) : m_string(s), m_tokenizer(s, {}) {
  m_iterator = m_tokenizer.begin();
}

auto Lexer::nextSymbol() -> Symbol {
  if (m_iterator == m_tokenizer.end()) {
    return Symbol{END, ""};
  }
  std::string t = *m_iterator;
  Type type;
  if (t == "&&")
    type = AND;
  else if (t == "||")
    type = OR;
  else if (t == "(")
    type = LEFT;
  else if (t == ")")
    type = RIGHT;
  else if (t == "!")
    type = NOT;
  else if (t == "true")
    type = TRUE_LITERAL;
  else if (t == "false")
    type = FALSE_LITERAL;
  else {
    // check if variable is valid
    const std::regex base_regex("^([^|()&! ]*)$");
    std::smatch base_match;

    if (!std::regex_match(t, base_match, base_regex)) {
      throw std::runtime_error("illegal variable encountered");
    } else {
      type = VAR;
    }
  }

  ++m_iterator;
  return Symbol{type, t};
}

}  // namespace DetailSelectionExprParser

SelectionExprParser::SelectionExprParser(Lexer lexer, bool defaultToChar)
    : m_lexer(std::move(lexer)), m_defaultToChar(defaultToChar) {}

StatusCode SelectionExprParser::build(
    std::unique_ptr<ISelectionAccessor>& accessor) {
  std::unique_ptr<ISelectionAccessor> root{nullptr};
  ANA_CHECK(expression(root));

  if (m_symbol.type != Lexer::END) {
    throw std::runtime_error(
        "Not all symbols in expression were consumed. Check your expression.");
  }

  accessor = std::move(root);
  return StatusCode::SUCCESS;
}

StatusCode SelectionExprParser::expression(
    std::unique_ptr<ISelectionAccessor>& root) {
  ANA_CHECK(term(root));
  while (m_symbol.type == Lexer::OR) {
    std::unique_ptr<ISelectionAccessor> left = std::move(root);
    ANA_CHECK(term(root));
    std::unique_ptr<ISelectionAccessor> right = std::move(root);
    root = std::make_unique<SelectionAccessorExprOr>(std::move(left),
                                                     std::move(right));
  }
  return StatusCode::SUCCESS;
}

StatusCode SelectionExprParser::term(
    std::unique_ptr<ISelectionAccessor>& root) {
  ANA_CHECK(factor(root));
  std::vector<std::unique_ptr<ISelectionAccessor>> factors;
  factors.push_back(std::move(root));

  while (m_symbol.type == Lexer::AND) {
    ANA_CHECK(factor(root));
    factors.push_back(std::move(root));
  }

  if (factors.size() == 1) {
    root = std::move(factors[0]);
  } else {
    root = std::make_unique<SelectionAccessorList>(std::move(factors));
  }
  return StatusCode::SUCCESS;
}

StatusCode SelectionExprParser::factor(
    std::unique_ptr<ISelectionAccessor>& root) {
  m_symbol = m_lexer.nextSymbol();
  if (m_symbol.type == Lexer::TRUE_LITERAL) {
    root = std::make_unique<SelectionAccessorNull>(true);
    m_symbol = m_lexer.nextSymbol();
  } else if (m_symbol.type == Lexer::FALSE_LITERAL) {
    root = std::make_unique<SelectionAccessorNull>(false);
    m_symbol = m_lexer.nextSymbol();
  } else if (m_symbol.type == Lexer::NOT) {
    ANA_CHECK(factor(root));
    std::unique_ptr<ISelectionAccessor> notEx =
        std::make_unique<SelectionAccessorExprNot>(std::move(root));
    root = std::move(notEx);
  } else if (m_symbol.type == Lexer::LEFT) {
    ANA_CHECK(expression(root));
    if (m_symbol.type != Lexer::RIGHT) {
      throw std::runtime_error(
          "Missing closing bracket, check your expression.");
    }
    m_symbol = m_lexer.nextSymbol();

  } else if (m_symbol.type == Lexer::VAR) {
    ANA_CHECK(makeSelectionAccessorVar(m_symbol.value, root, m_defaultToChar));
    m_symbol = m_lexer.nextSymbol();
  } else {
    throw std::runtime_error("Malformed expression.");
  }

  return StatusCode::SUCCESS;
}

}  // namespace CP
