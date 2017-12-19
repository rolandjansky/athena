// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/versions/HAuxContainer_v2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_HAUXCONTAINER_V2_H
#define DATAMODELTESTDATAREAD_HAUXCONTAINER_V2_H


#include "xAODCore/AuxContainerBase.h"
#include "AthContainers/PackedContainer.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


namespace DMTest {


class HAuxContainer_v2
  : public xAOD::AuxContainerBase
{
public:
  HAuxContainer_v2();

private:
  std::vector<float> aFloat;
};


} // namespace DMTest


SG_BASE (DMTest::HAuxContainer_v2, xAOD::AuxContainerBase);


#endif // not DATAMODELTESTDATAREAD_HAUXCONTAINER_V2_H

