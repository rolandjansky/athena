/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetEventAthenaPool/test/InDetSimDataCollectionCnv_p3_test.cxx
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 * @date 2020-05-22
 * @brief Regression tests.
 */

#undef NDEBUG

#include "../src/InDetSimDataCollectionCnv_p3.h"

#include "InDetSimDataCollectionCnv_common_test.h"


int main()
{
  return commonMain<InDetSimDataCollectionCnv_p3, InDetSimDataCollection_p3>();
}
