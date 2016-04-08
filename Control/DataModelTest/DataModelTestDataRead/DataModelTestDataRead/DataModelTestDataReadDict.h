// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataModelTestDataReadDict.h,v 1.3 2008-04-18 03:42:12 ssnyder Exp $

/**
 * @file  DataModelTestDataRead/DataModelTestDataReadDict.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Dictionary generation header.
 */

#ifndef DATAMODELTESTDATAREADDICT_H
#define DATAMODELTESTDATAREADDICT_H

#include "DataModelTestDataRead/BVec.h"
#include "DataModelTestDataRead/BDer.h"
#include "DataModelTestDataRead/DVec.h"
#include "DataModelTestDataRead/DDer.h"
#include "DataModelTestDataRead/ELVec.h"
#include "DataModelTestDataRead/G.h"
#include "DataModelTestDataRead/GVec.h"
#include "DataModelTestDataRead/GAuxContainer.h"
#include "DataModelTestDataRead/H.h"
#include "DataModelTestDataRead/HVec.h"
#include "DataModelTestDataRead/HView.h"
#include "DataModelTestDataRead/HAuxContainer.h"
#include "DataModelTestDataRead/versions/H_v1.h"
#include "DataModelTestDataRead/versions/HVec_v1.h"
#include "DataModelTestDataRead/versions/HView_v1.h"
#include "DataModelTestDataRead/versions/HAuxContainer_v1.h"

struct GCCXML_DUMMY_INSTANTIATION_DATAMODELTESTDATAREAD {
  ElementLink<DMTest::BVec> m_dum1;
  DataLink<DMTest::BVec> m_dum2;
};

#endif // not DATAMODELTESTDATAREADDICT_H

