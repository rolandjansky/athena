/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "CxxUtils/make_unique.h"
#include "boost/foreach.hpp"
#include <iostream>
#include <string>
#include <vector>


#include "larfcal_id_test_common.cxx"


class LArFCAL_ID_Test
  : public LArFCAL_ID, public ILArFCAL_ID_Test
{
public:
  virtual int get_lar_field_value() const override { return lar_field_value(); }
  virtual int get_lar_fcal_field_value() const override { return lar_fcal_field_value(); }
};


// neighbors
void test4 (const LArFCAL_ID& fcal_id)
{
  std::cout << "test4\n";

  std::vector<IdentifierHash> neighbourList;
  IdentifierHash hash_min = 999999 ;
  IdentifierHash hash_max = 0 ;
  for (unsigned int iCell = 0 ; iCell < fcal_id.channel_hash_max(); ++iCell){
    fcal_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList);

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
  std::unique_ptr<IdDictParser> parser = make_parser();
  IdDictMgr& idd = parser->m_idd;
  idd.add_metadata("FCAL2DNEIGHBORS",     "FCal2DNeighbors-DC3-05-Comm-01.txt");  
  idd.add_metadata("FCAL3DNEIGHBORSNEXT", "FCal3DNeighborsNext-DC3-05-Comm-01.txt");  
  idd.add_metadata("FCAL3DNEIGHBORSPREV", "FCal3DNeighborsPrev-DC3-05-Comm-01.txt");  
  std::unique_ptr<LArFCAL_ID> idhelper = make_helper<LArFCAL_ID_Test>(*parser);
  std::unique_ptr<LArFCAL_ID> idhelper_n = make_helper<LArFCAL_ID_Test>(*parser,true);
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
