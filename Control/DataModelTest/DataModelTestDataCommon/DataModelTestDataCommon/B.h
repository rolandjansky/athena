// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: B.h,v 1.2 2005-12-01 19:07:54 ssnyder Exp $

/**
 * @file  DataModelTestDataCommon/B.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is one of the two classes that will be contained in
 *        a @c DataVector.  This is the base class of the inheritance.
 */

#ifndef DMTESTCLASSES_B_H
#define DMTESTCLASSES_B_H


namespace DMTest {


/**
 * @brief Base contained class for @c DataVector tests.
 *
 *        Make sure this has a vtable and that it has data
 *        members.  That way, if we derive @c D from it virtually,
 *        there'll be an offset between @c B and @c D.
 */
struct B
{
  B (int b = 0) : m_x (b) {}
  virtual ~B() {}
  int m_x;
};


} // namespace DMTest


#endif // not DMTESTCLASSES_B_H
