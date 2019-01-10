// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/versions/CTrigAuxContainer_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_CTRIGAUXCONTAINER_V1_H
#define DATAMODELTESTDATACOMMON_CTRIGAUXCONTAINER_V1_H


#include "xAODTrigger/versions/ByteStreamAuxContainer_v1.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


namespace DMTest {


class CTrigAuxContainer_v1
  : public xAOD::ByteStreamAuxContainer_v1
{
public:
  CTrigAuxContainer_v1();

  void setName( const char* /*name*/ ) {}


private:
  std::vector<int> anInt;
  std::vector<float> aFloat;
};


} // namespace DMTest


SG_BASE (DMTest::CTrigAuxContainer_v1, xAOD::ByteStreamAuxContainer_v1);


#endif // not DATAMODELTESTDATACOMMON_CTRIGAUXCONTAINER_V1_H
