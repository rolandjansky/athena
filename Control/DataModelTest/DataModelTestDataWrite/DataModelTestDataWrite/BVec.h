// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BVec.h,v 1.3 2007-10-17 00:54:36 ssnyder Exp $

/**
 * @file  DataModelTestDataWrite/BVec.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is a @c DataVector containing the base class, @c B.
 */

#ifndef DATAMODELTESTDATAWRITE_BVEC_H
#define DATAMODELTESTDATAWRITE_BVEC_H


#include "DataModelTestDataCommon/B.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


/**
 * @brief A @c DataVector containing the base class, @c B.
 *
 * The redundant DMTest:: is required by the stupid pooliohander machinery.
 */
typedef DataVector<DMTest::B> BVec;


} // namespace DMTest


CLASS_DEF (DataVector<DMTest::B>, 9633, 1)


#endif // not DATAMODELTESTDATAWRITE_BVEC_H
