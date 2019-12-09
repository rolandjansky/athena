// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/CLinksAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKSAUXCONTAINER_H
#define DATAMODELTESTDATACOMMON_CLINKSAUXCONTAINER_H



#include "DataModelTestDataCommon/versions/CLinksAuxContainer_v1.h"


namespace DMTest {


typedef CLinksAuxContainer_v1 CLinksAuxContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CLinksAuxContainer, 1309326310, 1)


#endif // not DATAMODELTESTDATACOMMON_CLINKSAUXCONTAINER_H
