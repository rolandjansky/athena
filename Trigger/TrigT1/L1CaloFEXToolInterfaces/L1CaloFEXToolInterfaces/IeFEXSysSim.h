/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXSysSim.h  -
//                              -------------------
//     begin                : 23 03 2019
//     email                :  jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef IeFEXSysSim_H
#define IeFEXSysSim_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloEvent/CaloCellContainer.h"

namespace LVL1 {
  
/*
Interface definition for eFEXSysSim
*/

  static const InterfaceID IID_IeFEXSysSim("LVL1::IeFEXSysSim", 1, 0);

  class IeFEXSysSim : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;
    
    virtual StatusCode execute() = 0;

    virtual void init() = 0;

    virtual void cleanup() = 0;

    virtual int calcTowerID(int eta, int phi, int mod) = 0 ;
    
  private:

  };

  inline const InterfaceID& LVL1::IeFEXSysSim::interfaceID()
  {
    return IID_IeFEXSysSim;
  }

} // end of namespace

#endif
