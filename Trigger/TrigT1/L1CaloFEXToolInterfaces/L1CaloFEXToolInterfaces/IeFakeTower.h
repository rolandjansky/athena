/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEFAKETOWER_H
#define IEFAKETOWER_H
#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include <string>

namespace LVL1 {
  static const InterfaceID IID_IIeFakeTower("LVL1::IeFakeTower", 1 , 0);

  class IeFakeTower : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_IIeFakeTower; };
      virtual StatusCode init(std::string) = 0;
      virtual int getET(int, int, int, int, int) = 0;
      virtual StatusCode loadnext() = 0;
      virtual StatusCode execute() = 0;
      virtual StatusCode seteTowers(eTowerContainer*) = 0;

  };  
}

#endif