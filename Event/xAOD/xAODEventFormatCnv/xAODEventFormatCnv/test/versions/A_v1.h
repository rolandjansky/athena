// This file's extension implies that it's C, but it's really -*- C++ -*-.
/* Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration */

/**
 * @file xAODEventFormatCnv/test/versions/A_v1.h
 * @author Frank Berghaus <fberghaus@anl.gov>
 * @date Sep, 2020
 * @brief Class used for testing xAOD::EventFormat reading/writing.
 */
#ifndef XAODEVENTFORMATCNV_TEST_A_V1_H
#define XAODEVENTFORMATCNV_TEST_A_V1_H

#include "AthContainers/AuxElement.h"
#include "AthenaKernel/BaseInfo.h"


namespace xAODMakerTest {

class A_v1
  : public SG::AuxElement
{
public:
  float aFloat() const;
  void aFloat(float f);

  int anInt() const;
  void anInt(int i);

  const std::vector<int>& pVInt() const;
  void pVInt (const std::vector<int>&);
  void pVInt (std::vector<int>&&);

  const std::vector<float>& pVFloat() const;
  void pVFloat (const std::vector<float>&);
  void pVFloat (std::vector<float>&&);
};

} // namespace xAODMakerTest


SG_BASE(xAODMakerTest::A_v1, SG::AuxElement);

#endif  // XAODEVENTFORMATCNV_TEST_A_V1_H
