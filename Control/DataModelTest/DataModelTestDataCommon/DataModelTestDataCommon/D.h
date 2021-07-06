// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: D.h,v 1.2 2005-12-01 19:07:54 ssnyder Exp $

/**
 * @file  DataModelTestDataCommon/D.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is one of the two classes that will be contained in
 *        a @c DataVector.  This is the derived class of the inheritance.
 */

#include "DataModelTestDataCommon/B.h"

#ifndef DMTESTCLASSES_D_H
#define DMTESTCLASSES_D_H

namespace DMTest {


/**
 * @brief Derived contained class for @c DataVector tests.
 *
 *        This derives virtually from @c B.
 *        Actually, for now it doesn't; this because root6 i/o
 *        doesn't handle it properly.
 */
struct D
  : public /*virtual*/ B
{
  D (int d = 0) : B (d+1), m_x (d) {}
  // cppcheck-suppress duplInheritedMember
  int m_x;
};


} // namespace DMTest


#endif // not DMTESTCLASSES_D_H
