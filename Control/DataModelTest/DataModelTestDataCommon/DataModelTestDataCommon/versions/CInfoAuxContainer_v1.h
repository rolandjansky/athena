// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/versions/CInfoAuxContainer_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 *
 * This is for storing @c C objects standalone.
 */


#ifndef DATAMODELTESTDATACOMMON_CINFOAUXCONTAINER_V1_H
#define DATAMODELTESTDATACOMMON_CINFOAUXCONTAINER_V1_H


#include "xAODCore/AuxInfoBase.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


namespace DMTest {


class CInfoAuxContainer_v1
  : public xAOD::AuxInfoBase
{
public:
  CInfoAuxContainer_v1();

private:
  int anInt;
  float aFloat;
};


} // namespace DMTest


SG_BASE (DMTest::CInfoAuxContainer_v1, xAOD::AuxInfoBase);


#endif // not DATAMODELTESTDATACOMMON_CINFOAUXCONTAINER_V1_H
