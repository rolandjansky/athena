// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BDer.h,v 1.4 2007-10-17 01:01:47 ssnyder Exp $

/**
 * @file  DataModelTestDataRead/BVec.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Class used for testing the new @c DataVector inheritance scheme.
 *
 *        This is a class deriving from a @c DataVector
 *        containing the base class, @c B.
 */


#ifndef DATAMODELTESTDATAREAD_BDER_H
#define DATAMODELTESTDATAREAD_BDER_H


#include "DataModelTestDataRead/BVec.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


/**
 * @brief A class deriving from @c DataVector containing the base class, @c B.
 */
struct BDer
  : public BVec
{
private:
  ElementLink<BVec> m_el;
};


}

CLASS_DEF (DMTest::BDer, 9631, 1)

// Tell StoreGate that BDer derives from BVec.
SG_BASE(DMTest::BDer, DMTest::BVec);

#endif // not DATAMODELTESTDATAREAD_BDER_H
