/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/TileID_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for TileID.
 */

#undef NDEBUG

#include "CaloIdentifier/TileID.h"
#include "Identifier/IdentifierHash.h"
#include "CxxUtils/make_unique.h"
#include <iostream>


#include "tile_id_test_common.cxx"


class TileID_Test
  : public TileID, public ITile_ID_Test
{
public:
  virtual int get_tile_field_value() const override { return tile_field_value(); }
};


std::unique_ptr<TileID> make_helper (bool do_neighbours = false)
{
  auto idhelper = CxxUtils::make_unique<TileID_Test>();
  IdDictParser parser;
  parser.register_external_entity ("TileCalorimeter",
                                   "IdDictTileCalorimeter.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
  idd.add_metadata("TILENEIGHBORS",       "TileNeighbour_reduced.txt");  
  idhelper->set_do_neighbours (do_neighbours);
  assert (idhelper->initialize_from_dictionary (idd) == 0);

  assert (!idhelper->do_checks());
  idhelper->set_do_checks (true);
  assert (idhelper->do_checks());

  return idhelper;
}


void test_basic (const TileID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.cell_id (1, 1, 0, 0, 1));
  basic_print_id (idhelper, idhelper.cell_id (2, 1, 1, 10, 2));
  basic_print_id (idhelper, idhelper.cell_id (3, 1, 2, 11, 3));

  basic_print_id (idhelper, idhelper.cell_id (1, -1, 0, 0, 1));
  basic_print_id (idhelper, idhelper.cell_id (2, -1, 1, 10, 2));
  basic_print_id (idhelper, idhelper.cell_id (3, -1, 2, 11, 3));

  assert (idhelper.is_tile (idhelper.tile_system()));

  assert (idhelper.section (idhelper.tile_barrel()) == TileID::BARREL);
  assert (idhelper.section (idhelper.tile_extbarrel()) == TileID::EXTBAR);
  assert (idhelper.section (idhelper.tile_gap()) == TileID::GAPDET);

  assert (idhelper.section (idhelper.tile_det(3)) == 3);
  assert (idhelper.section (idhelper.tile_det(4)) == 4);
}


void test_neighbors (const TileID& idhelper)
{
  std::cout << "test_neighbors\n";

  std::vector<IdentifierHash> neighbourList;
  IdentifierHash hash_min = 999999 ;
  IdentifierHash hash_max = 0 ;
  for (unsigned int iCell = 0 ; iCell < idhelper.cell_hash_max(); ++iCell){
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
  assert (hash_max == idhelper.cell_hash_max()-1);
}


int main()
{
  std::string type = "TileCalorimeter";
  std::string xmlFile = "IdDictTileCalorimeter.xml";
  std::unique_ptr<IdDictParser> parser = make_parser(type, xmlFile);
  IdDictMgr& idd = parser->m_idd;
  idd.add_metadata("TILENEIGHBORS",       "TileNeighbour_reduced.txt");  
  std::unique_ptr<TileID> idhelper = make_helper<TileID_Test>(*parser);
  std::unique_ptr<TileID> idhelper_n = make_helper<TileID_Test>(*parser,true);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, false);
    test_exceptions (*idhelper);
    test_neighbors (*idhelper_n);
    test_to_string (*idhelper, false);
  }
  catch(TileID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
