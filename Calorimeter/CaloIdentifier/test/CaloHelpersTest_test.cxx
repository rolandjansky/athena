/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CaloIdentifier/test/CaloHelpersTest_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2018
 * @brief Unit test for CaloHelpersTest.
 */


#undef NDEBUG
#include "CaloIdentifier/CaloHelpersTest.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  CaloHelpersTest idhelpers;
  std::cout << "LArEM: "
            << idhelpers.emID().channel_hash_max() << " "
            << idhelpers.emID().region_hash_max() << "\n";
  std::cout << "LArHEC: "
            << idhelpers.hecID().channel_hash_max() << " "
            << idhelpers.hecID().region_hash_max() << "\n";
  std::cout << "LArFCAL: "
            << idhelpers.fcalID().channel_hash_max() << " "
            << idhelpers.fcalID().region_hash_max() << "\n";
  std::cout << "LArMinifcal: "
            << idhelpers.minifcalID().channel_hash_max() << " "
            << idhelpers.minifcalID().region_hash_max() << "\n";
  std::cout << "Tile: "
            << idhelpers.tileID().channel_hash_max() << " "
            << idhelpers.tileID().region_hash_max() << "\n";
  std::cout << "calo: "
            << idhelpers.caloID().calo_cell_hash_max() << " "
            << idhelpers.caloID().calo_region_hash_max() << "\n";
}


int main()
{
  std::cout << "CaloIdentifier/CaloHelpersTest\n";
  test1();
  return 0;
}
