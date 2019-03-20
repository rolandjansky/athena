/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "Identifier/IdentifierHash.h"
#include "CxxUtils/make_unique.h"
#include <iostream>


#include "tile_id_test_common.cxx"


class Tile_SuperCell_ID_Test
  : public Tile_SuperCell_ID, public ITile_ID_Test
{
public:
  virtual int get_tile_field_value() const override { return tile_field_value(); }
};


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
  std::string type = "TileCalorimeter";
  std::string xmlFile = "IdDictTileCalorimeter.xml";
  std::unique_ptr<IdDictParser> parser = make_parser (type, xmlFile);
  std::unique_ptr<Tile_SuperCell_ID> idhelper = make_helper<Tile_SuperCell_ID_Test>(*parser);
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
