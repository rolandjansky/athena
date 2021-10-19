/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IgFEXJetAlgo_H
#define IgFEXJetAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/gFEXJetTOB.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1 {

/*
Interface definition for eFEXegAlgo
*/

  static const InterfaceID IID_IgFEXJetAlgo("LVL1::IgFEXJetAlgo", 1, 0);
  typedef  std::array<std::array<int, 12>, 32> gTowersCentral;
  typedef  std::array<std::array<int, 7>, 32> gTowersForward;

  class IgFEXJetAlgo : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    virtual std::vector<std::unique_ptr<gFEXJetTOB>> largeRfinder(gTowersCentral Atwr, gTowersCentral Btwr,
                              gTowersForward CNtwr, gTowersForward CPtwr, int pucA, int pucB, int seedThreshold, int jetThreshold,
                              std::array<uint32_t, 7> & ATOB1_dat, std::array<uint32_t, 7> & ATOB2_dat,
                              std::array<uint32_t, 7> & BTOB1_dat, std::array<uint32_t, 7> & BTOB2_dat) = 0;


  };

  inline const InterfaceID& LVL1::IgFEXJetAlgo::interfaceID()
  {
    return IID_IgFEXJetAlgo;
  }

} // end of namespace

#endif
