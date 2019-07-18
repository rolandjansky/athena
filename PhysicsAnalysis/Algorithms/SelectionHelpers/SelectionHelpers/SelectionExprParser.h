/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTION_HELPERS__SELECTION_EXPR_PARSER_H
#define SELECTION_HELPERS__SELECTION_EXPR_PARSER_H

#include <boost/tokenizer.hpp>
#include <functional>
#include <memory>
#include <string>

namespace CP {

class SelectionExprParser {
 public:
  class BooleanExpression {
   public:
    using ValueGetter = std::function<bool(const std::string &)>;
    using Visitor = std::function<void(const BooleanExpression &)>;
    virtual bool evaluate(const ValueGetter &func) const = 0;
    virtual void visit(const Visitor &func) const = 0;
    virtual std::string toString() const = 0;
    virtual ~BooleanExpression() = default;
  };

  class Variable : public BooleanExpression {
   public:
    Variable(const std::string &name) : m_name(name) {}
    bool evaluate(const BooleanExpression::ValueGetter &func) const override {
      return func(m_name);
    }
    std::string toString() const override { return "Variable<" + m_name + ">"; }
    void visit(const BooleanExpression::Visitor &func) const override {
      func(*this);
    }

    const std::string &name() const { return m_name; }

   private:
    std::string m_name;
  };

 private:
  class Separator {
   public:
    bool operator()(std::string::const_iterator &next,
                    const std::string::const_iterator &end,
                    std::string &tok) const;

    void reset() {}
  };

 public:
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

 public:
  SelectionExprParser(Lexer lexer);
  std::unique_ptr<BooleanExpression> build();

 private:
  void expression();
  void term();
  void factor();

  Lexer m_lexer;
  Lexer::Symbol m_symbol;
  std::unique_ptr<BooleanExpression> m_root;
};

}  // namespace CP

#endif
