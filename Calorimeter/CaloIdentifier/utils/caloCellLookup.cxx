/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloIdentifier/utils/caloCellLookup.cxx
 * @author scott snyder
 * @date Apr 2020
 * @brief Command-line utility to decode a calorimeter cell hash.
 */


#include "CaloIdentifier/CaloHelpersTest.h"
#include <cstdlib>
#include <iostream>


int main (int argc, char** argv)
{
  CaloHelpersTest helpers;
  const CaloCell_ID& caloID = helpers.caloID();

  for (int i=1; i < argc; i++) {
    int hash = atoi (argv[i]);
    Identifier id = caloID.cell_id (hash);
    std::cout << hash << " " << id << " ";
    int subcalo = caloID.sub_calo (hash);
    switch (subcalo) {
    case CaloCell_ID::LAREM:
      std::cout << "LAREM " <<
        "BEC: " << helpers.emID().barrel_ec(id) << " "
        "Samp: "  << helpers.emID().sampling(id) << " "
        "Reg: "  << helpers.emID().region(id) << " "
        "Eta: "  << helpers.emID().eta(id) << " "
        "Phi: "  << helpers.emID().phi(id) << " "
        "SC: "  << helpers.emID().is_supercell(id) << std::endl;
      break;
    case CaloCell_ID::LARHEC:
      std::cout << "LARHEC " <<
        "P/N: " << helpers.hecID().pos_neg(id) << " "
        "Samp: "  << helpers.hecID().sampling(id) << " "
        "Reg: "  << helpers.hecID().region(id) << " "
        "Eta: "  << helpers.hecID().eta(id) << " "
        "Phi: "  << helpers.hecID().phi(id) << " "
        "SC: "  << helpers.hecID().is_supercell(id) << std::endl;
      break;
    case CaloCell_ID::LARFCAL:
      std::cout << "LARFCAL " <<
        "P/N: " << helpers.fcalID().pos_neg(id) << " "
        "Mod: "  << helpers.fcalID().module(id) << " "
        "Eta: "  << helpers.fcalID().eta(id) << " "
        "Phi: "  << helpers.fcalID().phi(id) << " "
        "SC: "  << helpers.fcalID().is_supercell(id) << std::endl;
      break;
    case CaloCell_ID::TILE:
      std::cout << "TILE " <<
        "Reg: " << helpers.tileID().region(id) << " "
        "Syst: " << helpers.tileID().system(id) << " "
        "Sect: " << helpers.tileID().section(id) << " "
        "Side: "  << helpers.tileID().side(id) << " "
        "Mod: "  << helpers.tileID().module(id) << " "
        "Tow: "  << helpers.tileID().tower(id) << " "
        "Samp: "  << helpers.tileID().sample(id) << " "
        "PMT: "  << helpers.tileID().pmt(id) << std::endl;
      break;
    case CaloCell_ID::LARMINIFCAL:
      std::cout << "LARMINIFCAL "
        "P/N: " << helpers.minifcalID().pos_neg(id) << " "
        "Mod: "  << helpers.minifcalID().module(id) << " "
        "Dep: "  << helpers.minifcalID().depth(id) << " "
        "Eta: "  << helpers.minifcalID().eta(id) << " "
        "Phi: "  << helpers.minifcalID().phi(id) << std::endl;
      break;
    default:
      std::cout << "UNKNOWN ";
    }
  }

  return 0;
}
