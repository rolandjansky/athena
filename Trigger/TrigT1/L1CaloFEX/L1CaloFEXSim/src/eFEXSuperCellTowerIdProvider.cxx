/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                            eFEXSuperCellTowerIdProvider.h - 
//                             -------------------
//     begin                : 25 09 2020
//     email                : tong.qiu@cern.ch
// ***************************************************************************/
#include "L1CaloFEXSim/eFEXSuperCellTowerIdProvider.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

LVL1::eFEXSuperCellTowerIdProvider::eFEXSuperCellTowerIdProvider(const std::string &type, const std::string &name, const IInterface *parent):
  AthAlgTool(type, name, parent)
{
  declareInterface<IeFEXSuperCellTowerIdProvider>(this);
  m_hascsvfile = false;
  std::string csvpath = getenv("WorkDir_DIR") + std::string("/share/sc_tower_map.csv");
  if (setAddress(csvpath) == StatusCode::FAILURE) {};
}

LVL1::eFEXSuperCellTowerIdProvider::~eFEXSuperCellTowerIdProvider()
{
}

StatusCode LVL1::eFEXSuperCellTowerIdProvider::setAddress(std::string inputaddress)
{
  m_hascsvfile = true;
  m_csvaddress = inputaddress;
  if ( loadcsv() == StatusCode::FAILURE) {
    m_hascsvfile = false;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXSuperCellTowerIdProvider::geteTowerIDandslot(uint64_t scid, int &eTowerID, int &slot) const
{
  auto mapentry = m_SuperCelltoTowerIdmap.find(scid);
  if (mapentry != m_SuperCelltoTowerIdmap.end()) {
    towerinfo towerinfo_tem = mapentry->second;
    eTowerID = towerinfo_tem.eTowerID;
    slot = towerinfo_tem.slot;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_ERROR("SuperCell " << scid << " is not available in " << m_csvaddress << ".");
  return StatusCode::FAILURE;
}

bool LVL1::eFEXSuperCellTowerIdProvider::hasSuperCell(uint64_t scid) const
{
  auto haskey = m_SuperCelltoTowerIdmap.find(scid);
  if (haskey == m_SuperCelltoTowerIdmap.end()) {
    return false;
  }
  return true;
}

bool LVL1::eFEXSuperCellTowerIdProvider::ifhaveinputfile() const {
  return m_hascsvfile;
}

StatusCode LVL1::eFEXSuperCellTowerIdProvider::loadcsv()
{
  // read the .csv file line by line
  std::string eachline;
  std::ifstream myfile(m_csvaddress);
  if (myfile.is_open()) {
    while (std::getline(myfile, eachline)) {
      // ignore text after #
      eachline = eachline.substr(0, eachline.find("#"));
      // prevent reading lines with only white spaces
      eachline = eachline.substr(0, eachline.find("\n"));
      bool if_emptyline = true;
      for (char const each_char : eachline) {
        // ignore empty lines
        if (!isspace(each_char)) {
          if_emptyline = false;
        }
      }
      if (if_emptyline) {
        continue;
      }

      // split lines in the .csv file by comma
      std::stringstream eachline_stream(eachline);
      int each_etowerid{ -1 }, each_slot{ -1 };
      std::string each_scid = "";
      int i{ 0 };
      while (eachline_stream.good()) {
        i++;
        int number_in_eachline;
        std::string tem_string;
        std::getline(eachline_stream, tem_string, ',');
        if (i == 1 || i == 3) {
          try {
            number_in_eachline = std::stoi(tem_string);
          }
          catch (...) {
            ATH_MSG_ERROR( "Invalid input in " << m_csvaddress << ".");
            return StatusCode::FAILURE;
          }
          if (i == 1) {
            each_etowerid = number_in_eachline;
          }
          else {
            each_slot = number_in_eachline;
          }
        }
        else if (i == 2) {
          each_scid = tem_string;
        }
        else {
          ATH_MSG_ERROR("Invalid input in " << m_csvaddress << ".");
          return StatusCode::FAILURE;
        }
      }
      if (each_etowerid > 601599 || each_etowerid < 100000) {
        ATH_MSG_ERROR("Invalid eTower ID in " << m_csvaddress << ".");
        return StatusCode::FAILURE;
      }
      if (each_slot > 13 || each_slot < 0) {
        ATH_MSG_ERROR("Invalid slot number in " << m_csvaddress << ".");
        return StatusCode::FAILURE;
      }
      if (each_scid.find("0x") == std::string::npos) {
        ATH_MSG_ERROR("Invalid SuperCell ID in " << m_csvaddress << ".");
        return StatusCode::FAILURE;
      }
      towerinfo each_towerinfo;
      each_towerinfo.eTowerID = each_etowerid;
      each_towerinfo.slot = each_slot;
      uint64_t scid_uint64 = strtoull(each_scid.c_str(), nullptr, 16);
      try {
      m_SuperCelltoTowerIdmap[scid_uint64] = each_towerinfo;
      } catch (...) {
        ATH_MSG_ERROR("Invalid SuperCell ID in " << m_csvaddress << ".");
        return StatusCode::FAILURE;
      }
    }
    myfile.close();
  }
  else {
    ATH_MSG_ERROR("eFEXFPGATowerIdProvider: Unable to open" << m_csvaddress << ".");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
