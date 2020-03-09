// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/CLinks.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKS_H
#define DATAMODELTESTDATACOMMON_CLINKS_H


#include "DataModelTestDataCommon/versions/CLinks_v1.h"


namespace DMTest {


typedef CLinks_v1 CLinks;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CLinks, 140140673, 1)


#endif // not DATAMODELTESTDATACOMMON_CLINKS_H
