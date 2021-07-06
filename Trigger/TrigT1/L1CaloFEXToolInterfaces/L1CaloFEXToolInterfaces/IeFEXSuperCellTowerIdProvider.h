/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEFEXSUPERCELLTOWERIDPROVIDER
#define IEFEXSUPERCELLTOWERIDPROVIDER
#include "GaudiKernel/IAlgTool.h"
#include "xAODTrigger/eFexEMRoI.h"
#include <string>

namespace LVL1 {
  static const InterfaceID IID_IIeFEXSuperCellTowerIdProvider("LVL1::IeFEXSuperCellTowerIdProvider", 1 , 0);

  class IeFEXSuperCellTowerIdProvider : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_IIeFEXSuperCellTowerIdProvider; };
      virtual StatusCode setAddress(std::string) = 0;
      virtual StatusCode geteTowerIDandslot(uint64_t, int&, int&, bool&) const = 0;
      virtual bool ifhaveinputfile() const = 0;

    private:
      virtual bool hasSuperCell(uint64_t) const = 0;
      virtual StatusCode loadcsv() = 0;
  };  
}

#endif