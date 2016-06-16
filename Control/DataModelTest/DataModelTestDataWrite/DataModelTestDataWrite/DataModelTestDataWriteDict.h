// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataModelTestDataWriteDict.h,v 1.3 2008-04-18 03:33:11 ssnyder Exp $

/**
 * @file  DataModelTestDataWrite/DataModelTestDataWriteDict.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Dictionary generation header.
 */

#ifndef DATAMODELTESTDATAWRITEDICT_H
#define DATAMODELTESTDATAWRITEDICT_H

#include "DataModelTestDataWrite/BVec.h"
#include "DataModelTestDataWrite/BDer.h"
#include "DataModelTestDataWrite/DVec.h"
#include "DataModelTestDataWrite/DDer.h"
#include "DataModelTestDataWrite/ELVec.h"
#include "DataModelTestDataWrite/G.h"
#include "DataModelTestDataWrite/GVec.h"
#include "DataModelTestDataWrite/GAuxContainer.h"
#include "DataModelTestDataWrite/H.h"
#include "DataModelTestDataWrite/HVec.h"
#include "DataModelTestDataWrite/HView.h"
#include "DataModelTestDataWrite/HAuxContainer.h"


struct GCCXML_DUMMY_INSTANTIATION_DATAMODELTESTDATAWRITE {
  ElementLink<DMTest::BVec> m_dum1;
  DataLink<DMTest::BVec> m_dum2;
};

#endif // not DATAMODELTESTDATAWRITEDICT_H

