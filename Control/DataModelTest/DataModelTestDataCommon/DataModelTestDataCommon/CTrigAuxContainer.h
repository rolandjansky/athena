// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/CTrigAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_CTRIGAUXCONTAINER_H
#define DATAMODELTESTDATACOMMON_CTRIGAUXCONTAINER_H


#include "DataModelTestDataCommon/versions/CTrigAuxContainer_v1.h"


namespace DMTest {


typedef CTrigAuxContainer_v1 CTrigAuxContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CTrigAuxContainer, 9725, 1)


#endif // not DATAMODELTESTDATACOMMON_CTRIGAUXCONTAINER_H
