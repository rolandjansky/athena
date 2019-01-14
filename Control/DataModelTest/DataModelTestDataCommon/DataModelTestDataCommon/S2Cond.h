// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/S2Cond.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Conditions declarations for S2.
 */


#ifndef DATAMODELTESTDATACOMMON_S2COND_H
#define DATAMODELTESTDATACOMMON_S2COND_H


#include "DataModelTestDataCommon/S2.h"
#include "DataModelTestDataCommon/S1Cond.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CondCont.h"


CONDCONT_DEF(DMTest::S2, 243030042, DMTest::S1);


#endif // not DATAMODELTESTDATACOMMON_S2COND_H
