/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/CaloCell_SuperCell_ID_test.cxx
 * @author scott snyder
 * @date Aug 2012
 * @brief Unit test for CaloCell_SuperCell_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/CaloCell_SuperCell_ID.h"


#include "calocell_id_test_common.cxx"


std::unique_ptr<CaloCell_SuperCell_ID> make_calo_id (IdDictParser& parser)
{
  return make_calo_id_t<CaloCell_SuperCell_ID,
                        LArEM_SuperCell_ID,
                        LArHEC_SuperCell_ID,
                        LArFCAL_SuperCell_ID,
                        Tile_SuperCell_ID> (parser);
}


int main()
{
  std::unique_ptr<IdDictParser> parser = make_parser();
  std::unique_ptr<CaloCell_SuperCell_ID> calo_id = make_calo_id(*parser);
  try {
    test_cells (*calo_id, true);
    test_sample (*calo_id, true);
    test_subcalo (*calo_id);
    test_regions (*calo_id);
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
