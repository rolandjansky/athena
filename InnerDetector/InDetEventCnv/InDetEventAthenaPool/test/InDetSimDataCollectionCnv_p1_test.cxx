/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetEventAthenaPool/test/InDetSimDataCollectionCnv_p1_test.cxx
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 * @date 2020-05-22
 * @brief Regression tests.
 */

#undef NDEBUG

#include "../src/InDetSimDataCollectionCnv_p1.h"

#include "InDetSimDataCollectionCnv_common_test.h"


int main ATLAS_NOT_THREAD_SAFE ()
{
  return commonMain<InDetSimDataCollectionCnv_p1, InDetSimDataCollection_p1>();
}
