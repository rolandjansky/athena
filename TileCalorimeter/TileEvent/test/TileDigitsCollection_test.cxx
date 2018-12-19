/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/test/TileDigitsCollection_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileDigitsCollection.
 */


#undef NDEBUG
#include "TileEvent/TileDigitsCollection.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  TileDigitsCollection coll (123);
  coll.setFragChipCRCWords (std::vector<uint32_t> { 1 });
  coll.setFragChipCRCWordsHigh (std::vector<uint32_t> { 2, 3 });
  coll.setFragChipHeaderWords (std::vector<uint32_t> { 3, 4, 5 });
  coll.setFragChipHeaderWordsHigh (std::vector<uint32_t> { 7, 6 });
  coll.setFragExtraWords (std::vector<uint32_t> { 9, 8 });
  coll.setFragSize (23);
  coll.setFragBCID (53);
  coll.setLvl1Id (1071);
  coll.setLvl1Type (1081);
  coll.setDetEvType (1091);
  coll.setRODBCID (1101);

  for (int i=0; i < 10; i++) {
    HWIdentifier hwid (10000 + i*1000);
    std::vector<float> digits { i*500 + 0.5f, i*500 + 1.5f,
                                 i*500 + 2.5f, i*500 + 3.5f };
    coll.push_back (std::make_unique<TileDigits> (hwid,
                                                  std::move (digits)));
  }

  TileDigitsCollection newColl (coll);
  assert (newColl.identify() == 123);
  assert (newColl.getFragChipCRCWords() ==  (std::vector<uint32_t> { 1 }));
  assert (newColl.getFragChipCRCWordsHigh() ==  (std::vector<uint32_t> { 2, 3 }));
  assert (newColl.getFragChipHeaderWords() == (std::vector<uint32_t> { 3, 4, 5 }));
  assert (newColl.getFragChipHeaderWordsHigh() ==  (std::vector<uint32_t> { 7, 6 }));
  assert (newColl.getFragExtraWords() == (std::vector<uint32_t> { 9, 8 }));
  assert (newColl.getFragSize() == 23);
  assert (newColl.getFragBCID() == 53);
  assert (newColl.getLvl1Id() == 1071);
  assert (newColl.getLvl1Type() == 1081);
  assert (newColl.getDetEvType() == 1091);
  assert (newColl.getRODBCID() == 1101);

  assert (newColl.size() == 10);
  for (int i=0; i < 10; i++) {
    const TileDigits* digit = newColl[i];
    assert (digit->adc_HWID() == HWIdentifier (10000 + i*1000));

    assert (digit->samples() == (std::vector<float> { i*500 + 0.5f, i*500 + 1.5f,
                                                       i*500 + 2.5f, i*500 + 3.5f }));
  }
}


int main()
{
  std::cout << "TileDigitsCollection_test\n";
  test1();
  return 0;
}
