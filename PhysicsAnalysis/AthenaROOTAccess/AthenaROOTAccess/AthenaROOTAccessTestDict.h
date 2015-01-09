// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccessAthenaROOTAccessTestDict.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Dictionary generation header.
 */


#include "AthenaROOTAccess/TestTypes.h"


struct ARATestInstan {
  ARATest::AVec avec;
  ARATest::CVec cvec;
  ARATest::DVec dvec;
  ARATest::EVec evec;
  ARATest::FVec fvec;
  ARATest::GVec gvec;
  ARATest::HVec hvec;
};
