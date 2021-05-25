/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1CaloFEXSim/eFakeTower.h"
#include <iostream>
#include <fstream>
#include <sstream>

// draft code to load the test vector from the online simulation.
// Each input file contains the ETs of all supercells in an FPGA.
// Events are separated by space. The block contains the mapping information.
// The code loads the mapping information first, then loads ETs event by event.
// The ETs are assiged to an eTowerContainer object.

LVL1::eFakeTower::eFakeTower(const std::string &type, const std::string &name, const IInterface *parent):
  AthAlgTool(type, name, parent)
{
  declareInterface<IeFakeTower>(this);
}

LVL1::eFakeTower::~eFakeTower() {
  for (auto& allfpga : m_alltowers) {
    delete allfpga.second;
  }
  for (auto& allfpga : m_dict) {
    delete allfpga.second;
  }
  m_alltowers.clear();
}

StatusCode LVL1::eFakeTower::init(std::string input_fileadress) {
  ATH_CHECK( m_eFEXFPGATowerIdProviderTool.retrieve() );
  m_numberofevents = 0;
  m_inputfile = input_fileadress;
  std::string txt = ".txt";
  for (int efex{ 0 }; efex < 24; efex++) {
    for (int fpga{ 0 }; fpga < 4; fpga++) {
      std::fstream fileStream;
      fileStream.open(m_inputfile + std::to_string(getFPGAnumber(efex, fpga)) + txt);
      if (fileStream.fail()) {
          continue;
      }
      ATH_CHECK( loaddic(getFPGAnumber(efex, fpga)) );
    }
  }
  return StatusCode::SUCCESS;
}

int LVL1::eFakeTower::getET(int FPGAid, int eta, int phi, int layer, int cell) {
  // find the ET of a supercell.
  if (eta > 5 || eta < 0) {
    ATH_MSG_ERROR( "Requested Supercell does not exist.");
    return 0;
  }
  if (phi > 9 || phi < 0) {
    ATH_MSG_ERROR( "Requested Supercell does not exist.");
    return 0;
  }
  if (layer > 4 || layer < 0) {
    ATH_MSG_ERROR( "Requested Supercell does not exist.");
    return 0;
  }
  if (cell > 3 || cell < 0) {
    ATH_MSG_ERROR( "Requested Supercell does not exist.");
    return 0;
  }
  if (m_dict.find(FPGAid) == m_dict.end()) {
    return 0;
  }
  int id = eta * 1000 + phi * 100 + layer * 10 + cell;
  if ((*m_alltowers[FPGAid]).find(id) == (*m_alltowers[FPGAid]).end()) {
    ATH_MSG_ERROR( "Trying to access uninitiated supercell.");
    return 0;
  }
  return (*m_alltowers[FPGAid])[id];
}

StatusCode LVL1::eFakeTower::loadnext()
{
  m_numberofevents++;
  // load the next events.
  for (auto& allfpga : m_alltowers) {
    delete allfpga.second;
  }
  std::string txt = ".txt";
  for (int efex{ 0 }; efex < 24; efex++) {
    for (int fpga{ 0 }; fpga < 4; fpga++) {
      std::fstream fileStream;
      fileStream.open(m_inputfile + std::to_string(getFPGAnumber(efex, fpga)) + txt);
      if (fileStream.fail()) {
          continue;
      }
      ATH_CHECK( loadFPGA(getFPGAnumber(efex, fpga)) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFakeTower::execute() {
    int FPGAtowerids[10][6];

  // set the Et of all towers to zero first
  for (int efex{ 0 }; efex < 24; efex++) {
    for (int fpga{ 0 }; fpga < 4; fpga++) {
      StatusCode sc = m_eFEXFPGATowerIdProviderTool->getRankedTowerIDinFPGA(efex, fpga, FPGAtowerids);
      if (sc ==  StatusCode::FAILURE) {
        return StatusCode::FAILURE;
      }
      for (int myrow = 0; myrow<10; myrow++){
        for (int mycol = 0; mycol<6; mycol++){
          LVL1::eTower* thistower = m_eTowerContainer->findTower(FPGAtowerids[myrow][mycol]);
          thistower->clearET();
        }
      }
    }
  }
  
  // replace all supercell energies in the eTowerContainer using the test vector.
  for (int efex{ 0 }; efex < 24; efex++) {
    for (int fpga{ 0 }; fpga < 4; fpga++) {
      StatusCode sc = m_eFEXFPGATowerIdProviderTool->getRankedTowerIDinFPGA(efex, fpga, FPGAtowerids);
      if (sc ==  StatusCode::FAILURE) {
        return StatusCode::FAILURE;
      }
      ATH_CHECK( changeFPGAET(FPGAtowerids, fpga, efex) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFakeTower::seteTowers(eTowerContainer* input) {
  m_eTowerContainer = input;
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFakeTower::changeFPGAET(int tmp_eTowersIDs_subset[][6], int FPGAnumber, int eFEXnumber) {
  // replace all supercell energies in the FPGA using the test vector.
  for (int myrow = 0; myrow<10; myrow++){
    for (int mycol = 0; mycol<6; mycol++){
      LVL1::eTower* thistower = m_eTowerContainer->findTower(tmp_eTowersIDs_subset[myrow][mycol]);
      
      // ignore umpty FPGA
      bool nothaveFPGA = m_dict.find(getFPGAnumber(eFEXnumber, FPGAnumber)) == m_dict.end();
      if (nothaveFPGA) {
        continue;
      }

      ATH_CHECK( changeTowerET(thistower, mycol, myrow, getFPGAnumber(eFEXnumber, FPGAnumber)) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFakeTower::changeTowerET(LVL1::eTower* inputtower, int eta, int phi, int FPGAid) {
  // update the ETs of the eTower using the values of test vector.
  inputtower->clearET();

  inputtower->setET(0 ,getET(FPGAid, eta, phi, 0, 0), 0);

  inputtower->setET(1, getET(FPGAid, eta, phi, 1, 0), 1);
  inputtower->setET(2, getET(FPGAid, eta, phi, 1, 1), 1);
  inputtower->setET(3, getET(FPGAid, eta, phi, 1, 2), 1);
  inputtower->setET(4, getET(FPGAid, eta, phi, 1, 3), 1);

  inputtower->setET(5, getET(FPGAid, eta, phi, 2, 0), 2);
  inputtower->setET(6, getET(FPGAid, eta, phi, 2, 1), 2);
  inputtower->setET(7, getET(FPGAid, eta, phi, 2, 2), 2);
  inputtower->setET(8, getET(FPGAid, eta, phi, 2, 3), 2);

  inputtower->setET(9, getET(FPGAid, eta, phi, 3, 0), 3);

  inputtower->setET(10, getET(FPGAid, eta, phi, 4, 0), 4);
  return StatusCode::SUCCESS;
}

StatusCode LVL1::eFakeTower::loadFPGA(int FPGAid) {
  std::string txt = ".txt";
  // load ETs of an FPGA and store them in m_alltowers.

  // Check if the mapping exists.
  if (m_dict.find(FPGAid) == m_dict.end()) {
    ATH_MSG_ERROR( "Mapping for FPGA "<< FPGAid << " does not exist!");
    return StatusCode::FAILURE;
  }
  std::vector<int>* Ets = loadBlock(m_inputfile + std::to_string(FPGAid) + txt, m_numberofevents);

  // check if the vector ETs have the same size as the mapping vector.
  if (Ets->size() != (*m_dict[FPGAid]).size()) {
    ATH_MSG_ERROR( "Unable to finish the tower mapping!" );
    return StatusCode::FAILURE;
  }
  std::unordered_map<int, unsigned int>* ETmap = new std::unordered_map<int, unsigned int>;
  for (unsigned int i = 0; i < Ets->size(); i++) {
    ETmap->insert(std::make_pair((*m_dict[FPGAid])[i], (*Ets)[i]));
  }
  m_alltowers[FPGAid] = ETmap;
  return StatusCode::SUCCESS;
}


StatusCode LVL1::eFakeTower::loaddic(int FPGAid) {
  // load mapping information and store it in the m_dict object.
  std::string txt = ".txt";
  std::vector<int>* dic0 = loadBlock(m_inputfile + std::to_string(FPGAid) + txt, 0);
  m_dict.insert(std::make_pair(FPGAid, dic0));
  return StatusCode::SUCCESS;
}

std::vector<int>* LVL1::eFakeTower::loadBlock(std::string inputfile, int eventnumber) {
  // load the eventnumber_th block of the input file.
  std::string eachline;
  std::ifstream myfile(inputfile);
  std::vector<int>* output = new std::vector<int>;
  if (myfile.is_open()) {
    int nblock = 0;
    while (std::getline(myfile, eachline)) {
      // The blocks are separated by empty lines.
      if (eachline.length() < 3) {
        nblock++;
      }
      if (nblock < eventnumber) {
        continue;
      }
      else if (nblock > eventnumber) {
        break;
      }
      std::string temvalue;
      std::stringstream ss(eachline);
      while (ss >> temvalue) {
        // the first block is always the dic
        if (eventnumber == 0) {
          output->push_back(std::stoi(temvalue));
        } else {
          int et = eFEXCompression::Expand(int(strtoull(temvalue.c_str(), nullptr, 16)));
          output->push_back(et);
        }
      }
    }
    myfile.close();
  }
  return output;
}

int LVL1::eFakeTower::getFPGAnumber(int iefex, int ifpga) {
  return iefex * 10 + ifpga;
}
