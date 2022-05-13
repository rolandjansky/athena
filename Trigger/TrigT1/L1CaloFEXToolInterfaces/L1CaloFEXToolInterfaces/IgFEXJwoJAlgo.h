/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  typedef  std::array<std::array<int, 8>, 32> gTowersForward;

  class IgFEXJwoJAlgo : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    virtual void setAlgoConstant(float aFPGA_A, float bFPGA_A,
                                 float aFPGA_B, float bFPGA_B,
                                 float aFPGA_C, float bFPGA_C,
                                 int gXE_seedThrA, int gXE_seedThrB, int gXE_seedThrC) = 0;

    virtual std::vector<std::unique_ptr<gFEXJwoJTOB>> jwojAlgo(gTowersCentral Atwr, gTowersCentral Btwr,
                                                              gTowersForward CNtwr, gTowersForward CPtwr,
                                                              std::array<uint32_t, 4> & outTOB) = 0;



  };

  inline const InterfaceID& LVL1::IgFEXJwoJAlgo::interfaceID()
  {
    return IID_IgFEXJwoJAlgo;
  }

} // end of namespace

#endif
