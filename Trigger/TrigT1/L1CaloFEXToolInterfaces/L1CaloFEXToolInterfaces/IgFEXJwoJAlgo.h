/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IgFEXJwoJAlgo_H
#define IgFEXJwoJAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/gFEXJwoJTOB.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1 {

/*
Interface definition for gFEXJwoJAlgo
*/

  static const InterfaceID IID_IgFEXJwoJAlgo("LVL1::IgFEXJwoJAlgo", 1, 0);
  typedef  std::array<std::array<int, 12>, 32> gTowersCentral;
  typedef  std::array<std::array<int, 7>, 32> gTowersForward;

  class IgFEXJwoJAlgo : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    virtual void setAlgoConstant(unsigned int aFPGA_A, unsigned int bFPGA_A,
                                 unsigned int aFPGA_B, unsigned int bFPGA_B,
                                 int gXE_seedThrA, int gXE_seedThrB) = 0;

    virtual std::vector<std::unique_ptr<gFEXJwoJTOB>> jwojAlgo(gTowersCentral Atwr, gTowersCentral Btwr,
                                                                            std::array<uint32_t, 4> & outTOB) = 0;


  };

  inline const InterfaceID& LVL1::IgFEXJwoJAlgo::interfaceID()
  {
    return IID_IgFEXJwoJAlgo;
  }

} // end of namespace

#endif
