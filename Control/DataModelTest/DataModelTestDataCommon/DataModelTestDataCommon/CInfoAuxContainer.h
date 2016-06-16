// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/CInfoAuxContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 *
 * This is for storing @c C objects standalone.
 */


#ifndef DATAMODELTESTDATACOMMON_CINFOAUXCONTAINER_H
#define DATAMODELTESTDATACOMMON_CINFOAUXCONTAINER_H


#include "DataModelTestDataCommon/versions/CInfoAuxContainer_v1.h"


namespace DMTest {


typedef CInfoAuxContainer_v1 CInfoAuxContainer;


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CInfoAuxContainer, 9723, 1)


#endif // not DATAMODELTESTDATACOMMON_CINFOAUXCONTAINER_H
