// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/CLinksContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKSCONTAINER_H
#define DATAMODELTESTDATACOMMON_CLINKSCONTAINER_H


#include "DataModelTestDataCommon/versions/CLinksContainer_v1.h"


namespace DMTest {


typedef CLinksContainer_v1 CLinksContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CLinksContainer, 1252897687, 1)


#endif // not DATAMODELTESTDATACOMMON_CLINKSCONTAINER_H
