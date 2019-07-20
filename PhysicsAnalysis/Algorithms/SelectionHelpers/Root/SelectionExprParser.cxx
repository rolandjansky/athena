/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionExprParser.h"
#include "SelectionHelpers/SelectionAccessorNull.h"

#include <iostream>
#include <regex>
#include <string>

namespace CP {
using namespace msgSelectionHelpers;

SelectionType SelectionAccessorExprBase::getBits(
    const SG::AuxElement& element) const {
  return getBool(element) ? selectionAccept() : selectionReject();
}

void SelectionAccessorExprBase::setBool(SG::AuxElement& /*element*/,
                                        bool /*value*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExprBase");
}

void SelectionAccessorExprBase::setBits(SG::AuxElement& /*element*/,
                                        SelectionType /*selection*/) const {
  throw std::runtime_error(
      "setting not supported for CP::SelectionAccessorExprBase");
}

SelectionAccessorExprAnd::SelectionAccessorExprAnd(
    std::unique_ptr<ISelectionAccessor> left,
    std::unique_ptr<ISelectionAccessor> right)
    : m_left(std::move(left)), m_right(std::move(right)) {}

bool SelectionAccessorExprAnd::getBool(const SG::AuxElement& element) const {
  return m_left->getBool(element) && m_right->getBool(element);
}

std::string SelectionAccessorExprAnd::label() const {
  return "( " + m_left->label() + " && " + m_right->label() + " )";
}

SelectionAccessorExprOr::SelectionAccessorExprOr(
    std::unique_ptr<ISelectionAccessor> left,
    std::unique_ptr<ISelectionAccessor> right)
    : m_left(std::move(left)), m_right(std::move(right)) {}

bool SelectionAccessorExprOr::getBool(const SG::AuxElement& element) const {
  return m_left->getBool(element) || m_right->getBool(element);
}

std::string SelectionAccessorExprOr::label() const {
  return "( " + m_left->label() + " || " + m_right->label() + " )";
}

SelectionAccessorExprNot::SelectionAccessorExprNot(
    std::unique_ptr<ISelectionAccessor> child)
    : m_child(std::move(child)) {}

bool SelectionAccessorExprNot::getBool(const SG::AuxElement& element) const {
  return !m_child->getBool(element);
}

std::string SelectionAccessorExprNot::label() const {
  return "!" + m_child->label();
}

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

namespace {
// typedef to much short name to make the code below a lot more readable
typedef DetailSelectionExprParser::Lexer Lexer;
}  // namespace

SelectionExprParser::SelectionExprParser(Lexer lexer, bool defaultToChar)
    : m_lexer(std::move(lexer)), m_defaultToChar(defaultToChar) {}

StatusCode SelectionExprParser::build(
    std::unique_ptr<ISelectionAccessor>& accessor) {
  ANA_CHECK(expression());

  if (m_symbol.type != Lexer::END) {
    throw std::runtime_error(
        "Not all symbols in expression were consumed. Check your expression.");
  }

  accessor = std::move(m_root);
  return StatusCode::SUCCESS;
}

StatusCode SelectionExprParser::expression() {
  ANA_CHECK(term());
  while (m_symbol.type == Lexer::OR) {
    std::unique_ptr<ISelectionAccessor> left = std::move(m_root);
    ANA_CHECK(term());
    std::unique_ptr<ISelectionAccessor> right = std::move(m_root);
    m_root = std::make_unique<SelectionAccessorExprOr>(std::move(left),
                                                       std::move(right));
  }
  return StatusCode::SUCCESS;
}

StatusCode SelectionExprParser::term() {
  ANA_CHECK(factor());
  while (m_symbol.type == Lexer::AND) {
    std::unique_ptr<ISelectionAccessor> left = std::move(m_root);
    ANA_CHECK(factor());
    std::unique_ptr<ISelectionAccessor> right = std::move(m_root);
    m_root = std::make_unique<SelectionAccessorExprAnd>(std::move(left),
                                                        std::move(right));
  }
  return StatusCode::SUCCESS;
}

StatusCode SelectionExprParser::factor() {
  m_symbol = m_lexer.nextSymbol();
  if (m_symbol.type == Lexer::TRUE_LITERAL) {
    m_root = std::make_unique<SelectionAccessorNull>(true);
    m_symbol = m_lexer.nextSymbol();
  } else if (m_symbol.type == Lexer::FALSE_LITERAL) {
    m_root = std::make_unique<SelectionAccessorNull>(false);
    m_symbol = m_lexer.nextSymbol();
  } else if (m_symbol.type == Lexer::NOT) {
    ANA_CHECK(factor());
    std::unique_ptr<ISelectionAccessor> notEx =
        std::make_unique<SelectionAccessorExprNot>(std::move(m_root));
    m_root = std::move(notEx);
  } else if (m_symbol.type == Lexer::LEFT) {
    ANA_CHECK(expression());
    if (m_symbol.type != Lexer::RIGHT) {
      throw std::runtime_error(
          "Missing closing bracket, check your expression.");
    }
    m_symbol = m_lexer.nextSymbol();

  } else if (m_symbol.type == Lexer::VAR) {
    ANA_CHECK(
        makeSelectionAccessorVar(m_symbol.value, m_root, m_defaultToChar));
    m_symbol = m_lexer.nextSymbol();
  } else {
    throw std::runtime_error("Malformed expression.");
  }

  return StatusCode::SUCCESS;
}
}  // namespace CP
