// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVec.h,v 1.2 2007-01-31 03:06:38 ssnyder Exp $

/**
 * @file  DataModelTestDataWrite/DVec.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is a @c DataVector containing the derived class, @c D.
 *        In this package, there is no inheritance relationship between
 *        the different @c DataVector classes.
 */

#ifndef DATAMODELTESTDATAWRITE_DVEC_H
#define DATAMODELTESTDATAWRITE_DVEC_H


#include "DataModelTestDataCommon/D.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


/**
 * @brief A @c DataVector containing the derived class, @c B.
 */
typedef DataVector<D> DVec;


} // namespace DMTest


CLASS_DEF (DataVector<DMTest::D>, 9634, 1)


#endif // not DATAMODELTESTDATAWRITE_DVEC_H
