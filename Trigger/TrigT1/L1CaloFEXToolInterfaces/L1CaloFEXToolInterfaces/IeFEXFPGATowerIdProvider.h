/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEFEXFPGATOWERIDPROVIDER_H
#define IEFEXFPGATOWERIDPROVIDER_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTrigger/eFexEMRoI.h"
#include <string>

namespace LVL1 {
  static const InterfaceID IID_IIeFEXFPGATowerIdProvider("LVL1::IeFEXFPGATowerIdProvider", 1 , 0);

  class IeFEXFPGATowerIdProvider : virtual public IAlgTool {

  public:
    static const InterfaceID& interfaceID() { return IID_IIeFEXFPGATowerIdProvider; };
    virtual StatusCode setAddress(std::string) = 0;
    virtual StatusCode getRankedTowerIDinFPGA(int, int, int(&)[10][6]) const = 0;
    virtual StatusCode getRankedTowerIDineFEX(int, int(&)[10][18]) const = 0;
    virtual bool ifhaveinputfile() const = 0;

  private:
    virtual StatusCode rankTowerinFPGA(int) = 0;
    virtual bool hasFPGA(int) const = 0;
    virtual StatusCode loadcsv() = 0;
    virtual int getFPGAIndex(int, int) const = 0;
  };  
}

#endif