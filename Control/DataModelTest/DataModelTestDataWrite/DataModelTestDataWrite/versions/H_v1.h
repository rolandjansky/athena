// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/versions/H_v1.h
 * @file H_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#ifndef DATAMODELTESTDATAWRITE_H_V1_H
#define DATAMODELTESTDATAWRITE_H_V1_H


#include "AthContainers/AuxElement.h"
#include "AthenaKernel/BaseInfo.h"


namespace DMTest {


class H_v1
  : public SG::AuxElement
{
public:
  int anInt() const;
  void setAnInt (int i);
};


} // namespace DMTest


SG_BASE (DMTest::H_v1, SG::AuxElement);


#endif // not DATAMODELTESTDATAWRITE_H_V1_H
