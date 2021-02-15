// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DataModelTestDataWrite/versions/GAuxContainer_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_GAUXCONTAINER_V1_H
#define DATAMODELTESTDATAWRITE_GAUXCONTAINER_V1_H


#include "xAODCore/AuxContainerBase.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


namespace DMTest {


class GAuxContainer_v1
  : public xAOD::AuxContainerBase
{
public:
  GAuxContainer_v1();

private:
  std::vector<int> anInt;
  std::vector<double> gFloat;
  std::vector<std::vector<double> > gvFloat;
};


} // namespace DMTest


SG_BASE (DMTest::GAuxContainer_v1, xAOD::AuxContainerBase);


#endif // not DATAMODELTESTDATAWRITE_GAUXCONTAINER_V1_H
