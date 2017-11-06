// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVec.h,v 1.2 2007-01-31 03:12:39 ssnyder Exp $

/**
 * @file  DataModelTestDataRead/DVec.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is a @c DataVector containing the derived class, @c D.
 *        We set things up so that in this package, @c DVec derives
 *        from @c BVec.
 */

#ifndef DATAMODELTESTDATAREAD_DVEC_H
#define DATAMODELTESTDATAREAD_DVEC_H


#include "DataModelTestDataCommon/D.h"
// Next include needed here to make sure we get the CLID
// for BVec.
#include "DataModelTestDataRead/BVec.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"


// Set up the inheritance relation between the @c DataVector classes.
DATAVECTOR_VIRTBASES1 (DMTest::D, DMTest::B);


namespace DMTest {


/**
 * @brief A @c DataVector containing the derived class, @c B.
 */
typedef DataVector<D> DVec;


}


CLASS_DEF (DataVector<DMTest::D>, 9634, 1)


#endif // not DATAMODELTESTDATAREAD_DVEC_H
