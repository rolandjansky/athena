// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/versions/HAuxContainer_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_HAUXCONTAINER_V1_H
#define DATAMODELTESTDATAWRITE_HAUXCONTAINER_V1_H


#include "xAODCore/AuxContainerBase.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


namespace DMTest {


class HAuxContainer_v1
  : public xAOD::AuxContainerBase
{
public:
  HAuxContainer_v1();

private:
  std::vector<int> anInt;
};


} // namespace DMTest


SG_BASE (DMTest::HAuxContainer_v1, xAOD::AuxContainerBase);


#endif // not DATAMODELTESTDATAWRITE_HAUXCONTAINER_V1_H

