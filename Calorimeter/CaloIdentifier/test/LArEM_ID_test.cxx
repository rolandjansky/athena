/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArEM_ID_test.cxx
 * @author scott snyder
 * @date Aug 2012
 * @brief Unit test for LArEM_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/LArEM_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "CxxUtils/make_unique.h"
#include <iostream>


#include "larem_id_test_common.cxx"


class LArEM_ID_Test
  : public LArEM_ID, public ILArEM_ID_Test
{
public:
  virtual int get_lar_field_value() const override { return lar_field_value(); }
  virtual int get_lar_em_field_value() const override { return lar_em_field_value(); }
};


void test_basic (const LArEM_Base_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (1, 1, 1, 0, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 1, 1, 2, 10));
  basic_print_id (idhelper, idhelper.channel_id (3, 2, 0, 0, 20));

  basic_print_id (idhelper, idhelper.channel_id (-1, 1, 1, 0, 1)); 
  basic_print_id (idhelper, idhelper.channel_id (-2, 1, 1, 2, 10));
  basic_print_id (idhelper, idhelper.channel_id (-3, 2, 0, 0, 20));
}


// neighbors
void test4 (const LArEM_ID& em_id)
{
  std::cout << "test4\n";

  std::vector<IdentifierHash> neighbourList;
  IdentifierHash hash_min = 999999 ;
  IdentifierHash hash_max = 0 ;
  for (unsigned int iCell = 0 ; iCell < em_id.channel_hash_max(); ++iCell){
    em_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList);

    for (IdentifierHash neighbourHash : neighbourList) {
      if(neighbourHash < hash_min ) {
        hash_min = neighbourHash;
      }
      if(neighbourHash > hash_max) {
        hash_max = neighbourHash;
      }
      
    }
  }

  assert (hash_min == 0);
  assert (hash_max == em_id.channel_hash_max()-1);
}


int main()
{
  std::unique_ptr<IdDictParser> parser = make_parser();
  std::unique_ptr<LArEM_ID> idhelper = make_helper<LArEM_ID_Test>(*parser);
  std::unique_ptr<LArEM_ID> idhelper_n = make_helper<LArEM_ID_Test>(*parser, true);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper);
    test_exceptions (*idhelper);
    test4 (*idhelper_n);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
