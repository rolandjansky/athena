/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetEventAthenaPool/test/InDetSimDataCollectionCnv_p2_test.cxx
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 * @date 2020-05-22
 * @brief Regression tests.
 */

#undef NDEBUG

#include "../src/InDetSimDataCollectionCnv_p2.h"

#include "InDetSimDataCollectionCnv_common_test.h"


int main()
{
  return commonMain<InDetSimDataCollectionCnv_p2, InDetSimDataCollection_p2>();
}
