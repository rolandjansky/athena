// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DDer.h,v 1.3 2007-01-31 03:12:39 ssnyder Exp $

/**
 * @file  DataModelTestDataRead/DDer.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is a class deriving from a @c DataVector
 *        containing the derived class, @c D.
 */


#ifndef DATAMODELTESTDATAREAD_DDER_H
#define DATAMODELTESTDATAREAD_DDER_H


#include "DataModelTestDataRead/DVec.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


/**
 * @brief A class deriving from @c DataVector
 *        containing the derived class, @c D.
 */
struct DDer
  : public DVec
{
};


}

CLASS_DEF (DMTest::DDer, 9632, 1)

// Tell StoreGate that DDer derives from DVec.
SG_BASE(DMTest::DDer, DMTest::DVec);

#endif // not DATAMODELTESTDATAREAD_DDER_H
