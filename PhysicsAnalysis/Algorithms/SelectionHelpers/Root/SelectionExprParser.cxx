/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectionHelpers/SelectionExprParser.h"

#include <iostream>
#include <regex>
#include <string>

namespace CP {

namespace {

using BooleanExpression = SelectionExprParser::BooleanExpression;
using BoolPtr = std::unique_ptr<BooleanExpression>;
using ValFunc = BooleanExpression::ValueGetter;
using Visitor = BooleanExpression::Visitor;

class True : public BooleanExpression {
 public:
  bool evaluate(const ValFunc&) const override { return true; }

  void visit(const Visitor& func) const override { func(*this); }

  std::string toString() const override { return "True<>"; }
};

class False : public BooleanExpression {
 public:
  bool evaluate(const ValFunc&) const override { return false; }
  void visit(const Visitor& func) const override { func(*this); }
  std::string toString() const override { return "False<>"; }
};

class Not : public BooleanExpression {
 public:
  Not(BoolPtr child) : m_child(std::move(child)) {}
  bool evaluate(const ValFunc& func) const override {
    return !m_child->evaluate(func);
  }
  std::string toString() const override {
    return "Not<" + m_child->toString() + ">";
  }
  void visit(const Visitor& func) const override {
    func(*this);
    m_child->visit(func);
  }

 private:
  BoolPtr m_child;
};

class Or : public BooleanExpression {
 public:
  Or(BoolPtr left, BoolPtr right)
      : m_left(std::move(left)), m_right(std::move(right)) {}

  bool evaluate(const ValFunc& func) const override {
    return m_left->evaluate(func) || m_right->evaluate(func);
  }

  std::string toString() const override {
    return "Or<" + m_left->toString() + "," + m_right->toString() + ">";
  }

  void visit(const Visitor& func) const override {
    func(*this);
    m_left->visit(func);
    m_right->visit(func);
  }

 private:
  BoolPtr m_left;
  BoolPtr m_right;
};

class And : public BooleanExpression {
 public:
  And(BoolPtr left, BoolPtr right)
      : m_left(std::move(left)), m_right(std::move(right)) {}

  bool evaluate(const ValFunc& func) const override {
    return m_left->evaluate(func) && m_right->evaluate(func);
  }
  std::string toString() const override {
    return "And<" + m_left->toString() + "," + m_right->toString() + ">";
  }
  void visit(const Visitor& func) const override {
    func(*this);
    m_left->visit(func);
    m_right->visit(func);
  }

 private:
  BoolPtr m_left;
  BoolPtr m_right;
};

}  // namespace


bool SelectionExprParser::Separator::operator()(
    std::string::const_iterator& next, const std::string::const_iterator& end,
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

SelectionExprParser::Lexer::Lexer(const std::string& s)
    : m_string(s), m_tokenizer(s, {}) {
  m_iterator = m_tokenizer.begin();
}

auto SelectionExprParser::Lexer::nextSymbol() -> Symbol {
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

SelectionExprParser::SelectionExprParser(Lexer lexer)
    : m_lexer(std::move(lexer)) {}

BoolPtr SelectionExprParser::build() {
  expression();
  return std::move(m_root);
}

void SelectionExprParser::expression() {
  term();
  while (m_symbol.type == Lexer::OR) {
    BoolPtr left = std::move(m_root);
    term();
    BoolPtr right = std::move(m_root);
    m_root = std::make_unique<Or>(std::move(left), std::move(right));
  }
}

void SelectionExprParser::term() {
  factor();
  while (m_symbol.type == Lexer::AND) {
    BoolPtr left = std::move(m_root);
    factor();
    BoolPtr right = std::move(m_root);
    m_root = std::make_unique<And>(std::move(left), std::move(right));
  }
}

void SelectionExprParser::factor() {
  m_symbol = m_lexer.nextSymbol();
  if (m_symbol.type == Lexer::TRUE_LITERAL) {
    m_root = std::make_unique<True>();
    m_symbol = m_lexer.nextSymbol();
  } else if (m_symbol.type == Lexer::FALSE_LITERAL) {
    m_root = std::make_unique<False>();
    m_symbol = m_lexer.nextSymbol();
  } else if (m_symbol.type == Lexer::NOT) {
    factor();
    BoolPtr notEx = std::make_unique<Not>(std::move(m_root));
    m_root = std::move(notEx);
  } else if (m_symbol.type == Lexer::LEFT) {
    expression();
    m_symbol = m_lexer.nextSymbol();
  } else if (m_symbol.type == Lexer::VAR) {
    m_root = std::make_unique<Variable>(m_symbol.value);
    m_symbol = m_lexer.nextSymbol();
  } else {
    throw std::runtime_error("Malformed expression.");
  }
}
}  // namespace CP
