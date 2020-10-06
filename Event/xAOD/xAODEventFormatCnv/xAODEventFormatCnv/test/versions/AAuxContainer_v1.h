// This file's extension implies that it's C, but it's really -*- C++ -*-.
/* Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration */
/**
 * @file xAODEventFormatCnv/test/versions/AAuxContainer_v1.h
 * @author frank berghaus <fberghaus@anl.gov>
 * @date May, 2020
 * @brief Class used for testing xAOD::EventFormat reading/writing.
 */
#ifndef XAODEVENTFORMATCNV_TEST_AAUXCONTAINER_V1_H
#define XAODEVENTFORMATCNV_TEST_AAUXCONTAINER_V1_H


#include "xAODCore/AuxContainerBase.h"
#include "AthContainers/PackedContainer.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


namespace xAODMakerTest {

class AAuxContainer_v1
  : public xAOD::AuxContainerBase
{
public:
  AAuxContainer_v1();

private:
  std::vector<int> anInt;
  std::vector<float> aFloat;
  SG::PackedContainer<std::vector<int> >  pVInt;
  SG::PackedContainer<std::vector<float> >  pVFloat;
};


}  // namespace xAODMakerTest


SG_BASE (xAODMakerTest::AAuxContainer_v1, xAOD::AuxContainerBase);


#endif  // XAODEVENTFORMATCNV_TEST_AAUXCONTAINER_V1_H
