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

class SelectionAccessorExprBase : public ISelectionAccessor {
  // leave getBool pure virtual
  // leave label pure virtual

 public:
  virtual SelectionType getBits(const SG::AuxElement &element) const override;

  virtual void setBool(SG::AuxElement & /*element*/,
                       bool /*value*/) const override;

  virtual void setBits(SG::AuxElement & /*element*/,
                       SelectionType /*selection*/) const override;
};

class SelectionAccessorExprAnd : public SelectionAccessorExprBase {
 public:
  SelectionAccessorExprAnd(std::unique_ptr<ISelectionAccessor> left,
                           std::unique_ptr<ISelectionAccessor> right);

  virtual bool getBool(const SG::AuxElement &element) const override;

  virtual std::string label() const override;

 private:
  std::unique_ptr<ISelectionAccessor> m_left;
  std::unique_ptr<ISelectionAccessor> m_right;
};

class SelectionAccessorExprOr : public SelectionAccessorExprBase {
 public:
  SelectionAccessorExprOr(std::unique_ptr<ISelectionAccessor> left,
                          std::unique_ptr<ISelectionAccessor> right);

  virtual bool getBool(const SG::AuxElement &element) const override;

  virtual std::string label() const override;

 private:
  std::unique_ptr<ISelectionAccessor> m_left;
  std::unique_ptr<ISelectionAccessor> m_right;
};

class SelectionAccessorExprNot : public SelectionAccessorExprBase {
 public:
  SelectionAccessorExprNot(std::unique_ptr<ISelectionAccessor> child);

  virtual bool getBool(const SG::AuxElement &element) const override;

  virtual std::string label() const override;

 private:
  std::unique_ptr<ISelectionAccessor> m_child;
};

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
  StatusCode expression();
  StatusCode term();
  StatusCode factor();

  DetailSelectionExprParser::Lexer m_lexer;
  DetailSelectionExprParser::Lexer::Symbol m_symbol;
  std::unique_ptr<ISelectionAccessor> m_root;
  bool m_defaultToChar;
};

}  // namespace CP

#endif
