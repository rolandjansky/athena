/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXFPGATowerIdProvider.h  -  
//                              -------------------
//     begin                : 25 09 2020
//     email                : tong.qiu@cern.ch
//  ***************************************************************************/
// Map the towers to FPGAs using infomation store in 
// Trigger_TrigT1_L1CaloFEX_L1CaloFEXSim_share_tower_fpga_efex_map.csv 

#ifndef EFEXFPGATOWERIDPROVIDER_H
#define EFEXFPGATOWERIDPROVIDER_H

#include "L1CaloFEXToolInterfaces/IeFEXFPGATowerIdProvider.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>
#include <unordered_map>
#include <vector>
namespace LVL1 {
  class eFEXFPGATowerIdProvider : public AthAlgTool, virtual public IeFEXFPGATowerIdProvider {
  struct towerinfo {
    int eTowerID;
    int eTowerEta;
    int eTowerPhi;
  };
  public:
    eFEXFPGATowerIdProvider(const std::string& type,const std::string& name,const IInterface* parent);
    ~eFEXFPGATowerIdProvider();
    StatusCode setAddress(std::string) override;
    StatusCode getRankedTowerIDinFPGA(int, int, int(&)[10][6]) const override;
    StatusCode getRankedTowerIDineFEX(int, int(&)[10][18]) const override;
    bool ifhaveinputfile() const override;

  private:
    bool m_hascsvfile;
    StatusCode rankTowerinFPGA(int) override;
    std::vector<std::vector<int>*> m_towerrankingcache;
    std::unordered_map<int, std::vector<towerinfo>* > m_alltowers;
    bool hasFPGA(int) const override;
    StatusCode loadcsv() override;
    int getFPGAIndex(int, int) const override;
    std::string m_csvaddress;
    Gaudi::Property<std::string> m_csvaddress_setting { this, "path", "xxx", "path to csv" };
  };
}

#endif
