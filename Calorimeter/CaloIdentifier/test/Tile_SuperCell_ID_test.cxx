/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/Tile_SuperCell_ID_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for Tile_SuperCell_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/Tile_SuperCell_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "Identifier/IdentifierHash.h"
#include <iostream>


#include "tile_id_test_common.cxx"


Tile_SuperCell_ID* make_helper()
{
  Tile_SuperCell_ID* idhelper = new Tile_SuperCell_ID;
  IdDictParser* parser = new IdDictParser;
  parser->register_external_entity ("TileCalorimeter",
                                    "IdDictTileCalorimeter.xml");
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  idhelper->set_do_neighbours (false);
  assert (idhelper->initialize_from_dictionary (idd) == 0);

  assert (!idhelper->do_checks());
  idhelper->set_do_checks (true);
  assert (idhelper->do_checks());

  return idhelper;
}


void test_basic (const Tile_SuperCell_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.cell_id (1, 1, 0, 0, 0));
  basic_print_id (idhelper, idhelper.cell_id (2, 1, 1, 10, 2));

  basic_print_id (idhelper, idhelper.cell_id (1, -1, 0, 0, 0));
  basic_print_id (idhelper, idhelper.cell_id (2, -1, 1, 10, 2));

  assert (idhelper.is_tile (idhelper.tile_system()));

  assert (idhelper.section (idhelper.tile_barrel()) == Tile_SuperCell_ID::BARREL);
  assert (idhelper.section (idhelper.tile_extbarrel()) == Tile_SuperCell_ID::EXTBAR);

  assert (idhelper.section (idhelper.tile_det(2)) == 2);
  assert (idhelper.is_supercell (idhelper.tile_det(2)));
}


int main()
{
  Tile_SuperCell_ID* idhelper = make_helper();
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, true);
    test_exceptions (*idhelper);
    test_to_string (*idhelper, true);
  }
  catch(TileID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}
