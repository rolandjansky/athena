/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                            eFEXSuperCellTowerIdProvider.h - 
//                             -------------------
//     begin                : 25 09 2020
//     email                : tong.qiu@cern.ch
// ***************************************************************************/
#ifndef EFEXSUPERCELLTOWERIDPROVIDER
#define EFEXSUPERCELLTOWERIDPROVIDER
#include "L1CaloFEXToolInterfaces/IeFEXSuperCellTowerIdProvider.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>
#include <unordered_map>

namespace LVL1 {
  class eFEXSuperCellTowerIdProvider: public AthAlgTool, virtual public IeFEXSuperCellTowerIdProvider{
  struct towerinfo {
    int eTowerID;
    int slot;
  };
  public:
    eFEXSuperCellTowerIdProvider(const std::string& type,const std::string& name,const IInterface* parent);
    ~eFEXSuperCellTowerIdProvider();
    StatusCode setAddress(std::string) override;
    StatusCode geteTowerIDandslot(uint64_t, int&, int&) const override;
    bool ifhaveinputfile() const override;
  private:
    bool m_hascsvfile;
    std::string m_csvaddress;
    bool hasSuperCell(uint64_t) const override;
    StatusCode loadcsv() override;
    std::unordered_map<uint64_t, towerinfo> m_SuperCelltoTowerIdmap;
  };
}
#endif