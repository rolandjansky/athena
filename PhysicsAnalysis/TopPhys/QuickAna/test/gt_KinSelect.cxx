/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/MessageCheck.h>
#include <AsgTools/UnitTest.h>
#include <QuickAna/KinObjectSelect.h>
#include <QuickAna/MessageCheck.h>
#include <gtest/gtest.h>
#include <xAODJet/Jet.h>

#ifdef ROOTCORE
#include <xAODRootAccess/Init.h>
#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using namespace ana;

//
// method implementations
//

using namespace asg::msgUserCode;

// helper function to make the momentum structure we need
ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> >
makeMom (float pt, float eta = 0, float phi = 0)
{
  ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > result;
  result.SetPt (pt);
  result.SetEta (eta);
  result.SetPhi (phi);
  return result;
}

// helper function to implement predicate tests
bool testSelect (bool pass, const ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> >& mom, const std::string& formula)
{
  std::unique_ptr<xAOD::Jet> jet;
  jet.reset (new xAOD::Jet);
  jet->makePrivateStore ();
  jet->setJetP4 (mom);
  KinObjectSelect select (formula);
  return select.select (*jet) == pass;
}



TEST (KinSelectTest, pt_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt > 40e3");
}

TEST (KinSelectTest, pt_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt > 60e3");
}

TEST (KinSelectTest, pt_float_fail)
{
  EXPECT_THROW_REGEX (KinObjectSelect select ("pt"), "didn't find boolean expression");
}

TEST (KinSelectTest, pt_fail_invalidToken)
{
  EXPECT_THROW_REGEX (KinObjectSelect select ("pt > 0 pt"), "invalid token");
}

TEST (KinSelectTest, eta_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3, 1.5), "eta < 1.6");
}

TEST (KinSelectTest, eta_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3, 1.5), "eta < 1.4");
}

TEST (KinSelectTest, phi_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3, 1.5, 0.5), "phi > 0.4");
}

TEST (KinSelectTest, phi_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3, 1.5, 0.5), "phi < 0.4");
}

TEST (KinSelectTest, and_true_true)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt > 1 && pt > 1");
}

TEST (KinSelectTest, and_true_false)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt > 1 && pt < 1");
}

TEST (KinSelectTest, and_false_true)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt < 1 && pt > 1");
}

TEST (KinSelectTest, and_false_false)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt < 1 && pt < 1");
}

TEST (KinSelectTest, and_true_true_true)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt > 1 && pt > 1 && pt > 1");
}

TEST (KinSelectTest, and_true_true_false)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt > 1 && pt > 1 && pt < 1");
}

TEST (KinSelectTest, and_false_true_true)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt < 1 && pt > 1 && pt > 1");
}

TEST (KinSelectTest, or_true_true)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt > 1 || pt > 1");
}

TEST (KinSelectTest, or_true_false)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt > 1 || pt < 1");
}

TEST (KinSelectTest, or_false_true)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt < 1 || pt > 1");
}

TEST (KinSelectTest, or_false_false)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt < 1 || pt < 1");
}

TEST (KinSelectTest, or_false_false_false)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "pt < 1 || pt < 1 || pt < 1");
}

TEST (KinSelectTest, or_false_false_true)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt < 1 || pt < 1 || pt > 1");
}

TEST (KinSelectTest, or_true_false_false)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "pt > 1 || pt < 1 || pt < 1");
}

TEST (KinSelectTest, and_or_fail)
{
  EXPECT_THROW_REGEX (KinObjectSelect select ("pt < 1 && pt < 1 || pt < 1"), "can't mix different logic operators without paranthesis");
}

TEST (KinSelectTest, or_and_fail)
{
  EXPECT_THROW_REGEX (KinObjectSelect select ("pt < 1 || pt < 1 && pt < 1"), "can't mix different logic operators without paranthesis");
}

TEST (KinSelectTest, parenthesis_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "(pt > 1)");
}

TEST (KinSelectTest, parenthesis_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "(pt < 1)");
}

TEST (KinSelectTest, parenthesis_nested_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "((pt > 1))");
}

TEST (KinSelectTest, parenthesis_nested_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (50e3), "((pt < 1))");
}

TEST (KinSelectTest, parenthesis_andDoubleOr_true_false_false_true)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "(pt > 1 || pt < 1) && (pt < 1 || pt > 1)");
}

TEST (KinSelectTest, parenthesis_orDoubleAnd_true_true_false_false)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "(pt > 1 && pt > 1) || (pt < 1 && pt < 1)");
}

TEST (KinSelectTest, parenthesis_or_false_true)
{
  EXPECT_PRED3 (testSelect, true, makeMom (50e3), "(pt < 1) || (pt > 1)");
}



const std::string congFormulaFull = "(pt > 20e3 && eta > -2.4 && eta < 2.4) || (pt > 30e3 && eta > 2.4 && eta < 4.5) || (pt > 30e3 && eta >-4.5 && eta < -2.4)";

TEST (CongTest, center_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (25e3, 0), congFormulaFull);
}

TEST (CongTest, center_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (15e3, 0), congFormulaFull);
}

TEST (CongTest, forward_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (35e3, 3), congFormulaFull);
}

TEST (CongTest, forward_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (25e3, 3), congFormulaFull);
}

TEST (CongTest, backward_pass)
{
  EXPECT_PRED3 (testSelect, true, makeMom (35e3, -3), congFormulaFull);
}

TEST (CongTest, backward_fail)
{
  EXPECT_PRED3 (testSelect, false, makeMom (25e3, -3), congFormulaFull);
}

int main (int argc, char **argv)
{
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif
  // msgKinSelect::setMsgLevel (MSG::Level::DEBUG);
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
