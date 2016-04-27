/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArFCAL_ID_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for LArFCAL_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/LArFCAL_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "boost/foreach.hpp"
#include <iostream>
#include <string>
#include <vector>


#include "larfcal_id_test_common.cxx"


LArFCAL_ID* make_helper (bool do_neighbours = false)
{
  LArFCAL_ID* idhelper = new LArFCAL_ID;
  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("LArCalorimeter",
                                    "IdDictLArCalorimeter_DC3-05-Comm-01.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  idd.add_metadata("FCAL2DNEIGHBORS",     "FCal2DNeighbors-DC3-05-Comm-01.txt");  
  idd.add_metadata("FCAL3DNEIGHBORSNEXT", "FCal3DNeighborsNext-DC3-05-Comm-01.txt");  
  idd.add_metadata("FCAL3DNEIGHBORSPREV", "FCal3DNeighborsPrev-DC3-05-Comm-01.txt");  
  idhelper->set_do_neighbours (do_neighbours);
  assert (idhelper->initialize_from_dictionary (idd) == 0);

  assert (!idhelper->do_checks());
  idhelper->set_do_checks (true);
  assert (idhelper->do_checks());

  return idhelper;
}


// neighbors
void test4 (const LArFCAL_ID& fcal_id)
{
  std::cout << "test4\n";

  std::vector<IdentifierHash> neighbourList;
  IdentifierHash hash_min = 999999 ;
  IdentifierHash hash_max = 0 ;
  for (unsigned int iCell = 0 ; iCell < fcal_id.channel_hash_max(); ++iCell){
    fcal_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList);

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
  assert (hash_max == fcal_id.channel_hash_max()-1);
}


void test_basic (const LArFCAL_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (2, 1, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 2, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 3, 1, 1));

  basic_print_id (idhelper, idhelper.channel_id (-2, 1, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (-2, 2, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (-2, 3, 1, 1));
}


int main()
{
  LArFCAL_ID* idhelper = make_helper();
  LArFCAL_ID* idhelper_n = make_helper(true);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, false);
    test_exceptions (*idhelper);
    test4 (*idhelper_n);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
