/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArHEC_ID_SuperCell_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for LArHEC_SuperCell_ID.
 */


#undef NDEBUG

#include "CaloIdentifier/LArHEC_SuperCell_ID.h"
#include "IdDictParser/IdDictParser.h"
#include <iostream>


#include "larhec_id_test_common.cxx"


LArHEC_SuperCell_ID* make_helper(bool do_neighbours = false)
{
  LArHEC_SuperCell_ID* idhelper = new LArHEC_SuperCell_ID;
  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("LArCalorimeter",
                                    "IdDictLArCalorimeter_DC3-05-Comm-01.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  idhelper->set_do_neighbours (do_neighbours);
  assert (idhelper->initialize_from_dictionary (idd) == 0);

  assert (!idhelper->do_checks());
  idhelper->set_do_checks (true);
  assert (idhelper->do_checks());

  return idhelper;
}


void test_basic (const LArHEC_Base_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (2, 0, 0, 0, 1)); 
  basic_print_id (idhelper, idhelper.channel_id (2, 0, 1, 3, 2)); 

  basic_print_id (idhelper, idhelper.channel_id (-2, 0, 0, 0, 1)); 
  basic_print_id (idhelper, idhelper.channel_id (-2, 0, 1, 3, 2)); 
}


void test_neighbors (const LArHEC_SuperCell_ID& idhelper)
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
  LArHEC_SuperCell_ID* idhelper = make_helper();
  LArHEC_SuperCell_ID* idhelper_n = make_helper(true);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, true);
    test_exceptions (*idhelper);
    test_neighbors (*idhelper_n);
    test_hec (*idhelper);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
