/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IgFEXaltMetAlgo_H
#define IgFEXaltMetAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1 {

/*
Interface definition for eFEXegAlgo
*/

  static const InterfaceID IID_IgFEXaltMetAlgo("LVL1::IgFEXaltMetAlgo", 1, 0);
  typedef  std::array<std::array<int, 12>, 32> gTowersCentral;
  typedef  std::array<std::array<int, 8>, 32>  gTowersForward;

  class IgFEXaltMetAlgo : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual void setAlgoConstant(std::vector<int>&& A_thr,
                                 std::vector<int>&& B_thr,
                                 const int rhoPlusThr) = 0;

    virtual void altMetAlgo(const gTowersCentral &Atwr, const gTowersCentral &Btwr,
                            std::array<uint32_t, 4> & outTOB) = 0;


  };

  
  inline const InterfaceID& LVL1::IgFEXaltMetAlgo::interfaceID()
  {
    return IID_IgFEXaltMetAlgo;
  }

} // end of namespace

#endif
