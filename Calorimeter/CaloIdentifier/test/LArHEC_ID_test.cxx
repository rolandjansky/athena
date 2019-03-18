/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArHEC_ID_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for LArHEC_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/LArHEC_ID.h"
#include "CxxUtils/make_unique.h"
#include "boost/foreach.hpp"
#include <iostream>


#include "larhec_id_test_common.cxx"


class LArHEC_ID_Test
  : public LArHEC_ID, public ILArHEC_ID_Test
{
public:
  virtual int get_lar_field_value() const override { return lar_field_value(); }
  virtual int get_lar_hec_field_value() const override { return lar_hec_field_value(); }
};


void test_basic (const LArHEC_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (2, 1, 0, 0, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 2, 0, 2, 10));
  basic_print_id (idhelper, idhelper.channel_id (2, 3, 1, 0, 20));

  basic_print_id (idhelper, idhelper.channel_id (-2, 1, 0, 0, 1));
  basic_print_id (idhelper, idhelper.channel_id (-2, 2, 0, 2, 10));
  basic_print_id (idhelper, idhelper.channel_id (-2, 3, 1, 0, 20));
}


void test_neighbors (const LArHEC_ID& idhelper)
{
  std::cout << "test_neighbors\n";

  std::vector<IdentifierHash> neighbourList;
  IdentifierHash hash_min = 999999 ;
  IdentifierHash hash_max = 0 ;
  for (unsigned int iCell = 0 ; iCell < idhelper.channel_hash_max(); ++iCell){
    idhelper.get_neighbours(iCell, LArNeighbours::all3D, neighbourList);

    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
    std::vector<IdentifierHash>::iterator last=neighbourList.end();
    for (;last!=first; first++){
      
      IdentifierHash neighbourHash=(*first);
      if(neighbourHash < hash_min ) {
        hash_min = neighbourHash;
      }
      if(neighbourHash > hash_max) {
        hash_max = neighbourHash;
      }
      
    }
  }

  assert (hash_min == 0);
  assert (hash_max == idhelper.channel_hash_max()-1);
}


int main()
{
  std::unique_ptr<IdDictParser> parser = make_parser();
  std::unique_ptr<LArHEC_ID> idhelper = make_helper<LArHEC_ID_Test>(*parser);
  std::unique_ptr<LArHEC_ID> idhelper_n = make_helper<LArHEC_ID_Test>(*parser,true);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, false);
    test_exceptions (*idhelper);
    test_neighbors (*idhelper_n);
    test_hec (*idhelper);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
