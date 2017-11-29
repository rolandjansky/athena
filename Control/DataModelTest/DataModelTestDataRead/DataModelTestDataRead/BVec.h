// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BVec.h,v 1.3 2007-10-17 01:01:47 ssnyder Exp $

/**
 * @file  DataModelTestDataRead/BVec.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is a @c DataVector containing the base class, @c B.
 */

#ifndef DATAMODELTESTDATAREAD_BVEC_H
#define DATAMODELTESTDATAREAD_BVEC_H


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


}


CLASS_DEF (DataVector<DMTest::B>, 9633, 1)


#endif // not DATAMODELTESTDATAREAD_BVEC_H
