// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/versions/CLinksContainer_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKSCONTAINER_V1_H
#define DATAMODELTESTDATACOMMON_CLINKSCONTAINER_V1_H


#include "DataModelTestDataCommon/versions/CLinks_v1.h"
#include "AthContainers/DataVector.h"


namespace DMTest {


typedef DataVector<CLinks_v1> CLinksContainer_v1;


} // namespace DMTest



#endif // not DATAMODELTESTDATACOMMON_CLINKSCONTAINER_V1_H
