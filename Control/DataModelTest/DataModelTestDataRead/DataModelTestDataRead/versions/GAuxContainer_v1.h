// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/versions/GAuxContainer_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#ifndef DATAMODELTESTDATAREAD_GAUXCONTAINER_V1_H
#define DATAMODELTESTDATAREAD_GAUXCONTAINER_V1_H


#include "xAODCore/AuxContainerBase.h"
#include "AthContainers/PackedContainer.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


namespace DMTest {


class GAuxContainer_v1
  : public xAOD::AuxContainerBase
{
public:
  GAuxContainer_v1();

private:
  SG::PackedContainer<int> anInt;
};


} // namespace DMTest


SG_BASE (DMTest::GAuxContainer_v1, xAOD::AuxContainerBase);


#endif // not DATAMODELTESTDATAREAD_GAUXCONTAINER_V1_H

