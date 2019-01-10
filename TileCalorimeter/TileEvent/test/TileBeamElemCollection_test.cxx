/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/test/TileBeamElemCollection_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileBeamElemCollection.
 */


#undef NDEBUG
#include "TileEvent/TileBeamElemCollection.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  TileBeamElemCollection coll (123);
  coll.setLvl1Id (1071);
  coll.setLvl1Type (1081);
  coll.setDetEvType (1091);
  coll.setRODBCID (1101);

  for (int i=0; i < 10; i++) {
    HWIdentifier hwid (10000 + i*1000);
    std::vector<uint32_t> digits { i*500u+1, i*500u+2, i*500u+3, i*500u+4 };
    coll.push_back (std::make_unique<TileBeamElem> (hwid,
                                                    std::move (digits)));
  }

  TileBeamElemCollection newColl (coll);
  assert (newColl.getLvl1Id() == 1071);
  assert (newColl.getLvl1Type() == 1081);
  assert (newColl.getDetEvType() == 1091);
  assert (newColl.getRODBCID() == 1101);

  assert (newColl.size() == 10);
  for (int i=0; i < 10; i++) {
    const TileBeamElem* digit = newColl[i];
    assert (digit->get_digits() == (std::vector<uint32_t> { i*500u+1, i*500u+2,
                                                            i*500u+3, i*500u+4 }));
  }
}


int main()
{
  std::cout << "TileBeamElemCollection_test\n";
  test1();
  return 0;
}
