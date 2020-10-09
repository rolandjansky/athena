/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SelectionHelpers/ISelectionAccessor.h>
#include <SelectionHelpers/SelectionAccessorChar.h>
#include <SelectionHelpers/SelectionAccessorBits.h>
#include <SelectionHelpers/SelectionAccessorList.h>
#include <SelectionHelpers/SelectionAccessorNull.h>
#include <AsgMessaging/MessageCheck.h>
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
    auto* selA = dynamic_cast<SelectionAccessorChar*>(accA.get());
    // check that this is actually nothing but a simple char accessor
    EXPECT_NE(selA, nullptr);
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
    // check that this is actually nothing but a simple bits accessor
    auto* selC = dynamic_cast<SelectionAccessorBits*>(accC.get());
    EXPECT_NE(selC, nullptr);
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

    // check AND of three accessors
    ASSERT_SUCCESS (makeSelectionAccessor ("a,as_char&&b,as_char&&c,as_bits", accAnd));
    auto* selAnd = dynamic_cast<SelectionAccessorList*>(accAnd.get());
    EXPECT_NE(selAnd, nullptr);

    accA->setBool (*jet, true);
    accB->setBool (*jet, true);
    accC->setBool (*jet, true);
    EXPECT_TRUE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)0);

    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    accC->setBool (*jet, true);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)1);

    accA->setBool (*jet, true);
    accB->setBool (*jet, false);
    accC->setBool (*jet, true);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)2);

    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    accC->setBool (*jet, true);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)3);

    accA->setBool (*jet, true);
    accB->setBool (*jet, true);
    accC->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)4);

    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    accC->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)5);

    accA->setBool (*jet, true);
    accB->setBool (*jet, false);
    accC->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)6);

    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    accC->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)7);

    // check an OR of two accessors
    std::unique_ptr<ISelectionAccessor> accOr;
    ASSERT_SUCCESS (makeSelectionAccessor ("a,as_char||b,as_char", accOr));
    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    EXPECT_TRUE (accOr->getBool (*jet));
    EXPECT_EQ (accOr->getBits (*jet), selectionAccept());
    accA->setBool (*jet, true);
    accB->setBool (*jet, false);
    EXPECT_TRUE (accOr->getBool (*jet));
    EXPECT_EQ (accOr->getBits (*jet), selectionAccept());
    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    EXPECT_FALSE (accOr->getBool (*jet));
    EXPECT_EQ (accOr->getBits (*jet), selectionReject());

    std::unique_ptr<ISelectionAccessor> accEx;
    ASSERT_SUCCESS (makeSelectionAccessor ("a,as_char||(b,as_char && c,as_bits)", accEx));
    accA->setBool (*jet, true);
    accB->setBool (*jet, true);
    accC->setBool (*jet, true);
    EXPECT_TRUE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionAccept());
    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    accC->setBool (*jet, true);
    EXPECT_TRUE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionAccept());
    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    accC->setBool (*jet, true);
    EXPECT_FALSE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionReject());
    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    accC->setBool (*jet, false);
    EXPECT_FALSE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionReject());
    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    accC->setBool (*jet, false);
    EXPECT_FALSE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionReject());
    accA->setBool (*jet, true);
    accB->setBool (*jet, false);
    accC->setBool (*jet, false);
    EXPECT_TRUE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionAccept());

    ASSERT_SUCCESS (makeSelectionAccessor ("a,as_char||(b,as_char && !c,as_bits)", accEx));
    accA->setBool (*jet, true);
    accB->setBool (*jet, true);
    accC->setBool (*jet, false);
    EXPECT_TRUE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionAccept());
    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    accC->setBool (*jet, false);
    EXPECT_TRUE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionAccept());
    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    accC->setBool (*jet, false);
    EXPECT_FALSE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionReject());
    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    accC->setBool (*jet, true);
    EXPECT_FALSE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionReject());
    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    accC->setBool (*jet, true);
    EXPECT_FALSE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionReject());
    accA->setBool (*jet, true);
    accB->setBool (*jet, false);
    accC->setBool (*jet, true);
    EXPECT_TRUE (accEx->getBool (*jet));
    EXPECT_EQ (accEx->getBits (*jet), selectionAccept());


    // test that an empty string produces a SelectionAccessorNull(true)
    std::unique_ptr<ISelectionAccessor> accEmpty;
    ASSERT_SUCCESS (makeSelectionAccessor ("", accEmpty));
    auto accNull = dynamic_cast<SelectionAccessorNull*>(accEmpty.get());
    EXPECT_NE (accNull, nullptr); // is in fact a null accessor
    // can either be true or false, let's test that it is true
    EXPECT_TRUE (accEmpty->getBool (*jet));

  }

  TEST (SelectionExprParser, tokenizer) {
    using tok = DetailSelectionExprParser::Tokenizer;

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
    using lexer = DetailSelectionExprParser::Lexer;
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
      std::string str = "alpha,as_bits && beta,as_char";
      lexer lex{str};
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
      std::string str = "alpha,as_bits&&beta,as_char";
      lexer lex{str};
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
      std::string str = "alpha,as_bits || beta,as_char";
      lexer lex{str};
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
      std::string str = "alpha,as_bits||beta,as_char";
      lexer lex{str};
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
    auto parse = [](std::string s) -> std::string {
      SelectionExprParser p(s, true);
      std::unique_ptr<ISelectionAccessor> acc;
      if(!p.build(acc).isSuccess()) {
        ADD_FAILURE() << "unable to parse expression";
      } 
      return acc->label();
    };

    std::string s;
    // this asserts the tree structure
    s = parse("A&&(C||!B)");
    EXPECT_EQ(s, "( A && ( C || !B ) )");
    // and this asserts the label output can be parse again,
    // leading to the exact same expression
    EXPECT_EQ(parse(s), s);

    s = parse("A&&(C||!(B&&true))");
    EXPECT_EQ(s, "( A && ( C || !( B && true ) ) )");
    EXPECT_EQ(parse(s), s);

    s = parse("alpha && ( beta || ! gamma )");
    EXPECT_EQ(s, "( alpha && ( beta || !gamma ) )");
    EXPECT_EQ(parse(s), s);

    s = parse(" alpha&&beta || !gamma ");
    EXPECT_EQ(s, "( ( alpha && beta ) || !gamma )");
    EXPECT_EQ(parse(s), s);

    s = parse(" (alpha&&beta) || !gamma ");
    EXPECT_EQ(s, "( ( alpha && beta ) || !gamma )");
    EXPECT_EQ(parse(s), s);

    s = parse(" A && B && C && D");
    EXPECT_EQ(s, "( A && B && C && D )");
    EXPECT_EQ(parse(s), s);


    EXPECT_THROW(parse("alpha &&"), std::runtime_error);
    EXPECT_THROW(parse("&&"), std::runtime_error);
    EXPECT_THROW(parse("&& alpha"), std::runtime_error);
    EXPECT_THROW(parse("alpha ||"), std::runtime_error);
    EXPECT_THROW(parse("||"), std::runtime_error);
    EXPECT_THROW(parse("|| alpha"), std::runtime_error);
    EXPECT_THROW(parse("(alpha && beta || gamma"), std::runtime_error);
    EXPECT_THROW(parse("alpha && beta) || gamma"), std::runtime_error);
    EXPECT_THROW(parse("!"), std::runtime_error);
    EXPECT_THROW(parse("alpha !"), std::runtime_error);
    EXPECT_THROW(parse("()"), std::runtime_error);
    EXPECT_THROW(parse(")"), std::runtime_error);
    EXPECT_THROW(parse("("), std::runtime_error);
  }
  
  TEST (SelectionExprParser, evaluate) {
    auto mkex = [](const std::string& s) {
      SelectionExprParser p(s, true);
      std::unique_ptr<ISelectionAccessor> acc;
      if(!p.build(acc).isSuccess()) {
        ADD_FAILURE() << "unable to parse expression";
      }
      return acc;
    };

    auto jet_ptr = std::make_unique<xAOD::Jet> ();
    auto& jet = *jet_ptr;
    jet.makePrivateStore();

    xAOD::Jet::Decorator<char> alpha("alpha");
    xAOD::Jet::Decorator<char> beta("beta");
    xAOD::Jet::Decorator<char> gamma("gamma");

    {

      std::string s = "alpha && ( beta || ! gamma )";
      auto ex = mkex(s);

      alpha(jet) = true;
      beta(jet) = true;
      gamma(jet) = true;
      EXPECT_EQ(ex->getBool(jet), true);

      alpha(jet) = false;
      EXPECT_EQ(ex->getBool(jet), 0);

      alpha(jet) = true;
      beta(jet) = false;
      EXPECT_EQ(ex->getBool(jet), 0);

      gamma(jet) = false;
      EXPECT_EQ(ex->getBool(jet), 1);
    }
    {
      std::string s = "true||(alpha && ( beta || ! gamma ))";
      auto ex = mkex(s);

      EXPECT_TRUE(ex->getBool(jet));

      alpha(jet) = false;
      EXPECT_TRUE(ex->getBool(jet));

      alpha(jet) = true;
      beta(jet) = false;
      EXPECT_TRUE(ex->getBool(jet));

      gamma(jet) = false;
      EXPECT_TRUE(ex->getBool(jet));
    }
    {
      auto ex = mkex("true");
      EXPECT_TRUE(ex->getBool(jet));

      ex = mkex("false");
      EXPECT_FALSE(ex->getBool(jet));

      ex = mkex("true && false");
      EXPECT_FALSE(ex->getBool(jet));

      ex = mkex("true || false");
      EXPECT_TRUE(ex->getBool(jet));

      ex = mkex("true && !false");
      EXPECT_TRUE(ex->getBool(jet));

      ex = mkex("!true && !false");
      EXPECT_FALSE(ex->getBool(jet));

    }
  }
}

ATLAS_GOOGLE_TEST_MAIN
