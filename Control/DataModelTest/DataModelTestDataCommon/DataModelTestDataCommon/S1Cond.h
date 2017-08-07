// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/S1Cond.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Conditions declarations for S1.
 */


#ifndef DATAMODELTESTDATACOMMON_S1COND_H
#define DATAMODELTESTDATACOMMON_S1COND_H


#include "DataModelTestDataCommon/S1.h"
#include "SGTools/BaseInfo.h"
#include "AthenaKernel/CondCont.h"


CLASS_DEF (CondCont<DMTest::S1>, 243030043, 0)
SG_BASE (CondCont<DMTest::S1>, CondContBase);


#endif // not DATAMODELTESTDATACOMMON_S1COND_H
