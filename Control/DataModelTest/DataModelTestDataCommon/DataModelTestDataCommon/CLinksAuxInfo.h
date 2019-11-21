// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/CLinksAuxInfo.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKSAUXINFO_H
#define DATAMODELTESTDATACOMMON_CLINKSAUXINFO_H


#include "DataModelTestDataCommon/versions/CLinksAuxInfo_v1.h"


namespace DMTest {


typedef CLinksAuxInfo_v1 CLinksAuxInfo;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CLinksAuxInfo, 87775542, 1)


#endif // not DATAMODELTESTDATACOMMON_CLINKSAUXINFO_H
