/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SelectionHelpers/ISelectionAccessor.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTesting/UnitTest.h>
#include <xAODJet/Jet.h>
#include <gtest/gtest.h>
#include "SelectionHelpers/SelectionExprParser.h"

//
// unit test
//

namespace CP
{
  TEST (ISelectionAccessorTest, all_tests)
  {
    auto jet = std::make_unique<xAOD::Jet> ();
    jet->makePrivateStore();

    // check a basic char accessor
    std::unique_ptr<ISelectionAccessor> accA;
    ASSERT_SUCCESS (makeSelectionAccessor ("a,as_char", accA));
    accA->setBool (*jet, false);
    EXPECT_EQ (jet->auxdata<char> ("a"), 0);
    accA->setBool (*jet, true);
    EXPECT_EQ (jet->auxdata<char> ("a"), 1);
    accA->setBits (*jet, selectionReject());
    EXPECT_EQ (jet->auxdata<char> ("a"), 0);
    accA->setBits (*jet, selectionAccept());
    EXPECT_EQ (jet->auxdata<char> ("a"), 1);

    // check an implicit char accessor
    std::unique_ptr<ISelectionAccessor> accB;
    ASSERT_SUCCESS (makeSelectionAccessor ("b", accB, true));
    accB->setBool (*jet, false);
    EXPECT_EQ (jet->auxdata<char> ("b"), 0);
    accB->setBool (*jet, true);
    EXPECT_EQ (jet->auxdata<char> ("b"), 1);

    // check a basic bits accessor
    std::unique_ptr<ISelectionAccessor> accC;
    ASSERT_SUCCESS (makeSelectionAccessor ("c,as_bits", accC));
    accC->setBool (*jet, false);
    EXPECT_EQ (jet->auxdata<SelectionType> ("c"), selectionReject());
    accC->setBool (*jet, true);
    EXPECT_EQ (jet->auxdata<SelectionType> ("c"), selectionAccept());
    accC->setBits (*jet, selectionReject());
    EXPECT_EQ (jet->auxdata<SelectionType> ("c"), selectionReject());
    accC->setBits (*jet, selectionAccept());
    EXPECT_EQ (jet->auxdata<SelectionType> ("c"), selectionAccept());

    // check an implicit bits accessor
    std::unique_ptr<ISelectionAccessor> accD;
    ASSERT_SUCCESS (makeSelectionAccessor ("d", accD, false));
    accD->setBool (*jet, false);
    EXPECT_EQ (jet->auxdata<SelectionType> ("d"), selectionReject());
    accD->setBool (*jet, true);
    EXPECT_EQ (jet->auxdata<SelectionType> ("d"), selectionAccept());

    // check an and of two accessors
    std::unique_ptr<ISelectionAccessor> accAnd;
    ASSERT_SUCCESS (makeSelectionAccessor ("a,as_char&&b,as_char", accAnd));
    accA->setBool (*jet, true);
    accB->setBool (*jet, true);
    EXPECT_TRUE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)0);
    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)1);
    accA->setBool (*jet, true);
    accB->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)2);
    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)3);
  TEST (SelectionExprParser, tokenizer) {
    using tok = SelectionExprParser::Tokenizer;

    std::string s = "A && B";
    tok tokens(s, {});
    auto it = tokens.begin();
    EXPECT_EQ(*(it++), "A");
    EXPECT_EQ(*(it++), "&&");
    EXPECT_EQ(*(it++), "B");

    s = "A&&B";
    tokens = tok{s, {}};
    it = tokens.begin();
    EXPECT_EQ(*(it++), "A");
    EXPECT_EQ(*(it++), "&&");
    EXPECT_EQ(*(it++), "B");

    s = "( alpha || gamma) &beta ";
    tokens = tok{s, {}};
    it = tokens.begin();
    EXPECT_EQ(*(it++), "(");
    EXPECT_EQ(*(it++), "alpha");
    EXPECT_EQ(*(it++), "||");
    EXPECT_EQ(*(it++), "gamma");
    EXPECT_EQ(*it, ")");
    EXPECT_THROW(it++, std::runtime_error);

    s = "a|b";
    tokens = tok{s, {}};
    it = tokens.begin();
    EXPECT_EQ(*it, "a");
    EXPECT_THROW(it++, std::runtime_error);

    s = "( alpha || gamma) &&beta ";
    tokens = tok{s, {}};
    it = tokens.begin();
    EXPECT_EQ(*(it++), "(");
    EXPECT_EQ(*(it++), "alpha");
    EXPECT_EQ(*(it++), "||");
    EXPECT_EQ(*(it++), "gamma");
    EXPECT_EQ(*(it++), ")");
    EXPECT_EQ(*(it++), "&&"); // & not a token: considered part of variable
    EXPECT_EQ(*(it++), "beta");

  }

  TEST (SelectionExprParser, lexer) {
    using lexer = SelectionExprParser::Lexer;
    using type = lexer::Type;

    lexer::Symbol s;
    {
      lexer lex("A&&B");
      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "A");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::AND);
      EXPECT_EQ(s.value, "&&");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "B");
    }

    {
      lexer lex{"alpha,as_bits && beta,as_char"};
      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "alpha,as_bits");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::AND);
      EXPECT_EQ(s.value, "&&");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "beta,as_char");
    }


    {
      lexer lex{"alpha,as_bits&&beta,as_char"};
      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "alpha,as_bits");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::AND);
      EXPECT_EQ(s.value, "&&");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "beta,as_char");
    }

    {
      lexer lex{"alpha,as_bits || beta,as_char"};
      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "alpha,as_bits");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::OR);
      EXPECT_EQ(s.value, "||");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "beta,as_char");
    }


    {
      lexer lex{"alpha,as_bits||beta,as_char"};
      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "alpha,as_bits");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::OR);
      EXPECT_EQ(s.value, "||");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "beta,as_char");
    }

    {
      std::string str = "alpha,as_bits||(beta,as_char &&!gamma,as_bits)";
      lexer lex{str};
      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "alpha,as_bits");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::OR);

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::LEFT);

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "beta,as_char");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::AND);

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::NOT);

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::VAR);
      EXPECT_EQ(s.value, "gamma,as_bits");

      s = lex.nextSymbol();
      EXPECT_EQ(s.type, type::RIGHT);
    }
  }

  TEST (SelectionExprParser, parser) {
    auto parse = [](std::string s) {
      SelectionExprParser p(s);
      auto ex = p.build(); 
      return ex->toString();
    };

    // this asserts the tree structure
    EXPECT_EQ(parse("A&&(C||!B)"),
              "And<Variable<A>,Or<Variable<C>,Not<Variable<B>>>>");
    EXPECT_EQ(parse("A&&(C||!(B&&true)"),
              "And<Variable<A>,Or<Variable<C>,Not<And<Variable<B>,True<>>>>>");

    EXPECT_EQ(parse("alpha && ( beta || ! gamma )"),
              "And<Variable<alpha>,Or<Variable<beta>,Not<Variable<gamma>>>>");

    EXPECT_EQ(parse(" alpha&&beta || !gamma "),
              "Or<And<Variable<alpha>,Variable<beta>>,Not<Variable<gamma>>>");

    EXPECT_EQ(parse(" (alpha&&beta) || !gamma "),
              "Or<And<Variable<alpha>,Variable<beta>>,Not<Variable<gamma>>>");
  }
  
  TEST (SelectionExprParser, evaluate) {
    std::unordered_map<std::string, bool> values = {
        {"alpha", true}, {"beta", true}, {"gamma", true}};
    auto val = [&](const std::string& key) { return values.at(key); };
    auto mkex = [](const std::string& s) {
      SelectionExprParser p(s);
      return p.build();
    };

    {
      std::string s = "alpha && ( beta || ! gamma )";
      auto ex = mkex(s);

      EXPECT_EQ(ex->evaluate(val), true);

      values["alpha"] = false;
      EXPECT_EQ(ex->evaluate(val), false);

      values["alpha"] = true;
      values["beta"] = false;
      EXPECT_EQ(ex->evaluate(val), false);

      values["gamma"] = false;
      EXPECT_EQ(ex->evaluate(val), true);
    }
    {
      std::string s = "true||(alpha && ( beta || ! gamma ))";
      auto ex = mkex(s);

      EXPECT_EQ(ex->evaluate(val), true);

      values["alpha"] = false;
      EXPECT_EQ(ex->evaluate(val), true);

      values["alpha"] = true;
      values["beta"] = false;
      EXPECT_EQ(ex->evaluate(val), true);

      values["gamma"] = false;
      EXPECT_EQ(ex->evaluate(val), true);
    }
    {
      auto ex = mkex("true");
      EXPECT_EQ(ex->evaluate(val), true);

      ex = mkex("false");
      EXPECT_EQ(ex->evaluate(val), false);

      ex = mkex("true && false");
      EXPECT_EQ(ex->evaluate(val), false);

      ex = mkex("true || false");
      EXPECT_EQ(ex->evaluate(val), true);

      ex = mkex("true && !false");
      EXPECT_EQ(ex->evaluate(val), true);

      ex = mkex("!true && !false");
      EXPECT_EQ(ex->evaluate(val), false);

    }
  }
}

ATLAS_GOOGLE_TEST_MAIN
