/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                            eFakeTower.h - 
//                             -------------------
//     begin                : 25 09 2020
//     email                : tong.qiu@cern.ch
// ***************************************************************************/

#ifndef EFAKETOWER_H
#define EFAKETOWER_H
#include<string>
#include<unordered_map>
#include<vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IeFEXSysSim.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/eFEXFPGATowerIdProvider.h"
#include "L1CaloFEXToolInterfaces/IeFakeTower.h"
#include "L1CaloFEXSim/eFEXCompression.h"


namespace LVL1 {
  class eFakeTower: public AthAlgTool, virtual public IeFakeTower
  {
  public:
    eFakeTower(const std::string& type,const std::string& name,const IInterface* parent);
    virtual StatusCode init(std::string) override;
    ~eFakeTower();
    virtual int getET(int, int, int, int, int) override;
    virtual StatusCode loadnext() override;
    virtual StatusCode execute() override;
    virtual StatusCode seteTowers(eTowerContainer*) override;

  private:
    StatusCode changeFPGAET(int tmp_eTowersIDs_subset[][6], int, int);
    StatusCode loadFPGA(int);
    StatusCode loaddic(int);
    StatusCode changeTowerET(LVL1::eTower*, int, int, int);
    int getFPGAnumber(int, int);
    eTowerContainer* m_eTowerContainer;
    std::vector<int>* loadBlock(std::string, int);
    int m_numberofevents;
    // const eFEXFPGATowerIdProvider m_eFEXFPGATowerIdProviderTool;
    ToolHandle<IeFEXFPGATowerIdProvider> m_eFEXFPGATowerIdProviderTool {this, "eFEXFPGATowerIdProviderTool", "LVL1::eFEXFPGATowerIdProvider", "Tool that provides tower-FPGA mapping"};
    std::string m_inputfile;
    
    // map of all supercell ETs of FPGAs
    // m_alltowers[FPGAid] = (supercell id,ET)
    // supercell id = eta * 1000 + phi * 100 + layer * 10 + cell
    std::unordered_map<int, std::unordered_map<int, unsigned int>*> m_alltowers;
    
    // map for mapping infomation
    // m_dict[FPGAid] = [ list of supercell id in order ]
    std::unordered_map<int, std::vector<int>*> m_dict;
  };
}

#endif