/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/CaloCell_ID_test.cxx
 * @author scott snyder
 * @date Aug 2012
 * @brief Unit test for CaloCell_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/CaloCell_ID.h"


#include "calocell_id_test_common.cxx"


std::unique_ptr<CaloCell_ID> make_calo_id (IdDictParser& parser,
                                           bool do_neighbours = false)
{
  return make_calo_id_t<CaloCell_ID,
                        LArEM_ID,
                        LArHEC_ID,
                        LArFCAL_ID,
                        TileID> (parser, do_neighbours);
}


void test_neighbours (const CaloCell_ID& calo_id)
{
  std::cout << "test_neighbours\n";
  
  for (unsigned int iCell = 0 ; iCell < calo_id.calo_cell_hash_max(); ++iCell){
    Identifier cellId = calo_id.cell_id(iCell);
    IdentifierHash hash_min = 888888 ;
    IdentifierHash hash_max = 0 ;
    calo_id.calo_cell_hash_range(cellId, hash_min, hash_max);
    std::vector<IdentifierHash> neighbourList;
    assert (calo_id.get_neighbours(iCell, LArNeighbours::all3D, neighbourList)
            == 0);
    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
    std::vector<IdentifierHash>::iterator last=neighbourList.end();
    for (;last!=first; first++){
	  
      IdentifierHash neighbourHash=(*first);
      assert (hash_min <= neighbourHash && neighbourHash <= hash_max);
    }
  }
}


int main()
{
  std::unique_ptr<IdDictParser> parser = make_parser();
  std::unique_ptr<CaloCell_ID> calo_id = make_calo_id(*parser);
  std::unique_ptr<CaloCell_ID> calo_id_n = make_calo_id(*parser, true);
  try {
    test_cells (*calo_id);
    test_sample (*calo_id, false);
    test_subcalo (*calo_id);
    test_regions (*calo_id);
    test_neighbours (*calo_id_n);
    test_exceptions (*calo_id);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  catch(TileID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
