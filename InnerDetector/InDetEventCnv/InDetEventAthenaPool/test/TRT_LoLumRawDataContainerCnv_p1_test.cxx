/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetEventAthenaPool/test/TRT_LoLumRawDataContainerCnv_p1_test.cxx
 * @brief Regression tests.
 */

#undef NDEBUG

#include "../src/TRT_LoLumRawDataContainerCnv_p1.h"

#include "TRT_LoLumRawDataContainerCnv_common_test.h"


int main ATLAS_NOT_THREAD_SAFE ()
{
  return commonMain<TRT_LoLumRawDataContainerCnv_p1, InDetRawDataContainer_p1>();
}
