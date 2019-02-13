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
    accA->setBool (*jet, true);
    accB->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)1);
    accA->setBool (*jet, false);
    accB->setBool (*jet, true);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)2);
    accA->setBool (*jet, false);
    accB->setBool (*jet, false);
    EXPECT_FALSE (accAnd->getBool (*jet));
    EXPECT_EQ (accAnd->getBits (*jet), ~(SelectionType)3);
  }
}

ATLAS_GOOGLE_TEST_MAIN
