/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1CaloFEXSim/eFEXFPGATowerIdProvider.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include "PathResolver/PathResolver.h"

LVL1::eFEXFPGATowerIdProvider::eFEXFPGATowerIdProvider(const std::string &type, const std::string &name, const IInterface *parent):
  AthAlgTool(type, name, parent)
{
  declareInterface<IeFEXFPGATowerIdProvider>(this);
}

StatusCode LVL1::eFEXFPGATowerIdProvider::initialize()
{
  m_hascsvfile = false;
  std::string csvpath =  PathResolver::find_file("tower_fpga_efex_map.csv", "DATAPATH");
  if (setAddress(csvpath) == StatusCode::FAILURE) {
    ATH_MSG_WARNING("tower_fpga_efex_map.csv missing or invalid. Swiching to hard-coded mapping.");
  };
  return StatusCode::SUCCESS;   
}

LVL1::eFEXFPGATowerIdProvider::~eFEXFPGATowerIdProvider()
{
  for (auto each : m_alltowers) {
    delete each.second;
  }
  m_alltowers.clear();
  for (auto each : m_towerrankingcache) {
    delete each;
    each = nullptr;
  }
  m_hascsvfile = false;
}

StatusCode LVL1::eFEXFPGATowerIdProvider::setAddress(std::string inputaddress) 
{
  if (inputaddress.empty()) {
    m_hascsvfile = false;
    return StatusCode::FAILURE;
  }
  m_hascsvfile = true;
  m_csvaddress = inputaddress;
  if (loadcsv() == StatusCode::FAILURE) {
    m_hascsvfile = false;
    return StatusCode::FAILURE;
  }
  m_towerrankingcache = std::vector<std::vector<int>*>(96, nullptr);
  // sort the towers in each FPGA
  for (int efex{ 0 }; efex < 24; efex++) {
    for (int fpga{ 0 }; fpga < 4; fpga++) {
      if (rankTowerinFPGA(getFPGAIndex(efex, fpga)) == StatusCode::FAILURE) {
        m_hascsvfile = false;
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXFPGATowerIdProvider::getRankedTowerIDinFPGA(int eFEXID, int FPGAID, int(&towerlist)[10][6]) const {
  int FPGAindex{ getFPGAIndex(eFEXID, FPGAID) };
  if (m_towerrankingcache[FPGAindex]) {
    // obtain the tower order from cache
    int vectorindex{ 0 };
    for (int i{ 0 }; i < 10; i++) {
      for (int j{ 0 }; j < 6; j++) {
        towerlist[i][j] = (*m_towerrankingcache[FPGAindex]).at(vectorindex++);
      }
    }
  }
  // set tower ids to 0 if the information about the FPGA stored in the .csv file is damaged
  else {
    for (int i{ 0 }; i < 10; i++) {
      for (int j{ 0 }; j < 6; j++) {
        towerlist[i][j] = 0;
      }
    }
    ATH_MSG_ERROR("FPGA mapping info missing.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFEXFPGATowerIdProvider::getRankedTowerIDineFEX(int eFEXID, int(&towerlist)[10][18]) const {
  int FPGA0index{ getFPGAIndex(eFEXID, 0) };
  int FPGA1index{ getFPGAIndex(eFEXID, 1) };
  int FPGA2index{ getFPGAIndex(eFEXID, 2) };
  int FPGA3index{ getFPGAIndex(eFEXID, 3) };

  if (!m_towerrankingcache[FPGA0index] || !m_towerrankingcache[FPGA1index] ||
      !m_towerrankingcache[FPGA2index] || !m_towerrankingcache[FPGA3index]) {
    for (int i{ 0 }; i < 10; i++) {
      for (int j{ 0 }; j < 18; j++) {
        towerlist[i][j] = 0;
      }
    }
    ATH_MSG_ERROR("FPGA mapping info missing.");
    return StatusCode::FAILURE; 
  }

  // FPGA0
  int vectorindex{ 0 };
  for (int i{ 0 }; i < 10; i++) {
    for (int j{ 0 }; j < 6; j++) {
      towerlist[i][j] = (*m_towerrankingcache[FPGA0index]).at(vectorindex++);
    }
  }

  // fPGA1 skipping overlap with FPGA0 and FPGA2
  vectorindex = 0;
  for (int i{ 0 }; i < 10; i++) {
    vectorindex += 2;
    for (int j{ 2 }; j < 4; j++) {
      towerlist[i][j + 4] = (*m_towerrankingcache[FPGA1index]).at(vectorindex++);
    }
    vectorindex += 2;
  }
  
  // FPGA2
  vectorindex = 0;
  for (int i{ 0 }; i < 10; i++) {
    for (int j{ 0 }; j < 6; j++) {
      towerlist[i][j + 8] = (*m_towerrankingcache[FPGA2index]).at(vectorindex++);
    }
  }

  // FPGA4 skipping overlap with FPGA2
  vectorindex = 0;
  for (int i{ 0 }; i < 10; i++) {
    vectorindex += 2;
    for (int j{ 2 }; j < 6; j++) {
      towerlist[i][j + 12] = (*m_towerrankingcache[FPGA3index]).at(vectorindex++);
    }
  }
  return StatusCode::SUCCESS;
}

bool LVL1::eFEXFPGATowerIdProvider::ifhaveinputfile() const {
  return m_hascsvfile;
}

StatusCode LVL1::eFEXFPGATowerIdProvider::rankTowerinFPGA(int FPGAindex)
{  
  // This function determines the order of towers in an FPGA.
  // Towers are sorted using eta and phi, and 
  // then store the result in m_towerrankingcache object for future use.

  // If the tower order of an FPGA has not been determined yet, start sorting.
  if (!m_towerrankingcache[FPGAindex]) {
    std::vector<std::pair<int, int>> rankingmap;
    // the row order is determined by eta while the column order is determined by phi
    for (auto each : *m_alltowers[FPGAindex]) {
      if (each.eTowerPhi < 3 && each.eTowerPhi > -1 && FPGAindex >= getFPGAIndex(21, 0)) {
        rankingmap.push_back(std::pair<int, int>(-each.eTowerEta - (90 + each.eTowerPhi) * 100, each.eTowerID));
      } else {
        rankingmap.push_back(std::pair<int, int>(-each.eTowerEta - each.eTowerPhi * 100, each.eTowerID));
      }
    }
    // There is supposed to be 60 towers in each FPGA
    if (rankingmap.size() != 60) {
      return StatusCode::FAILURE;
    }
    std::sort(rankingmap.begin(), rankingmap.end(),
      [](std::pair<int, int> a, std::pair<int, int> b) {
        // tower around eta == 0 has the same eta.
        if (a.first == b.first) {
          return (a.second < b.second);
        }
        return (a.first > b.first);
      });
    std::vector<int>* output = new std::vector<int>;
    int vectorindex = 0;
    for (int i{ 0 }; i < 10; i++) {
      for (int j{ 0 }; j < 6; j++) {
        output->push_back(rankingmap[vectorindex++].second);
      }
    }
    m_towerrankingcache[FPGAindex] = output;
  }
  return StatusCode::SUCCESS;
}

bool LVL1::eFEXFPGATowerIdProvider::hasFPGA(int FPGAindex) const
{
  // check if the info of a speific FPGA has been loaded or not
  std::unordered_map<int, std::vector<towerinfo>*>::const_iterator haskey = m_alltowers.find(FPGAindex);
  if (haskey == m_alltowers.end()) {
    return false;
  }
  return true;
}

StatusCode LVL1::eFEXFPGATowerIdProvider::loadcsv()
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
        // ignore lines with non-digit characters
        if (!isspace(each_char) && !isdigit(each_char) && each_char != ',') {
          ATH_MSG_INFO("invalid line");
          break;
        }
        // ignore empty lines
        if (!isspace(each_char)) {
          if_emptyline = false;
          //break;
        }
      }
      if (if_emptyline) {
        continue;
      }

      // split lines in the .csv file by comma
      std::stringstream eachline_stream(eachline);
      std::vector<int> numbers_in_eachline;
      while (eachline_stream.good()) {
        std::string tem_string;
        std::getline(eachline_stream, tem_string, ',');
        try {
          numbers_in_eachline.push_back(std::stoi(tem_string));
        }
        catch (...) {
          ATH_MSG_WARNING( "Invalid input in tower_fpga_efex_map.csv." );
           return StatusCode::FAILURE;
        }
      }

      // There is supposed to be 5 numbers in each line: 
      // eFEX ID, FPGA ID, eTower ID, eta, phi
      if (numbers_in_eachline.size() != 5) {
        ATH_MSG_WARNING( "Invalid input in tower_fpga_efex_map.csv." );
        return StatusCode::FAILURE;
      }
      // eta and phi are supposed to be positive
      if (numbers_in_eachline[3] < 0 || numbers_in_eachline[4] < 0) {
        ATH_MSG_WARNING("Invalid input in tower_fpga_efex_map.csv.");
        // return scodefail
        return StatusCode::FAILURE;
      }
      int FPGAindex = getFPGAIndex(numbers_in_eachline[0], numbers_in_eachline[1]);
      // check the range of eFEX ID and FPGA ID
      if (FPGAindex == -1) {
        ATH_MSG_WARNING("Invalid input in tower_fpga_efex_map.csv.");
        return StatusCode::FAILURE;
      }
      towerinfo tem_towerinfo;
      tem_towerinfo.eTowerID = numbers_in_eachline[2];
      // determine the sign of eta using the tower id
      // positive: 200000-299999, 400000-499999, 600000-699999
      // negative: 100000-199999, 300000-399999, 500000-599999
      int etasign;
      if (int(numbers_in_eachline[2] / 100000) % 2 != 0) {
        etasign = -1;
      } else {
        etasign = 1;
      }
      tem_towerinfo.eTowerEta = etasign * (numbers_in_eachline[3]);
      tem_towerinfo.eTowerPhi = numbers_in_eachline[4];

      // Create a key for the FPGA if the the FPGA key is not 
      // avaliable in the m_alltowers hash table object yet.
      // Otherwise append the tower to the existing bucket.
      if (hasFPGA(FPGAindex)) {
        m_alltowers[FPGAindex]->push_back(tem_towerinfo);
      } else {
        std::vector<towerinfo>* FPGAinfovector = new std::vector<towerinfo>;
        FPGAinfovector->push_back(tem_towerinfo);
        m_alltowers[FPGAindex] = FPGAinfovector;
      }
    }
    myfile.close();
  } else {
    ATH_MSG_WARNING("Unable to open share/tower_fpga_efex_map.csv.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

int LVL1::eFEXFPGATowerIdProvider::getFPGAIndex(int eFEXid, int FPGAid) const
{
  // check the range of eFEX ID and FPGA ID
  // There is supposed to be 24 eFex and each eFex
  // contains 4 FPGAs.
  if (FPGAid < 0 or FPGAid > 3) {
    return -1;
  }
  if (eFEXid < 0 or FPGAid > 23) {
    return -1;
  }
  return eFEXid * 4 + FPGAid;
}
