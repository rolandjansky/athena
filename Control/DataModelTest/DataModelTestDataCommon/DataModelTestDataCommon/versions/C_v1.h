// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/versions/C_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_C_V1_H
#define DATAMODELTESTDATACOMMON_C_V1_H


#include "AthContainers/AuxElement.h"
#include "AthenaKernel/BaseInfo.h"


namespace DMTest {


class C_v1
  : public SG::AuxElement
{
public:
  float aFloat() const;
  void setAFloat (float f);

  int anInt() const;
  void setAnInt (int i);

  unsigned int pInt() const;
  void setPInt (unsigned int);

  float pFloat() const;
  void setPFloat (float);

  const std::vector<int>& pvInt() const;
  void setPVInt (const std::vector<int>&);
  void setPVInt (std::vector<int>&&);

  const std::vector<float>& pvFloat() const;
  void setPVFloat (const std::vector<float>&);
  void setPVFloat (std::vector<float>&&);
};


} // namespace DMTest


SG_BASE (DMTest::C_v1, SG::AuxElement);


#endif // not DATAMODELTESTDATACOMMON_C_V1_H
