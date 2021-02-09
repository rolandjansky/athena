/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXSim.h  -
//                              -------------------
//     begin                : 23 03 2019
//     email                :  jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef IjFEXSim_H
#define IjFEXSim_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloEvent/CaloCellContainer.h"

namespace LVL1 {
  
/*
Interface definition for jFEXSim
*/

  static const InterfaceID IID_IjFEXSim("LVL1::IjFEXSim", 1, 0);

  class IjFEXSim : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual void init(int id) = 0;

    virtual void reset() = 0;

    virtual void execute() = 0;
    virtual int ID() = 0;
    virtual void SetTowersAndCells_SG(int tmp[16*2][17]) = 0;
    virtual void SetTowersAndCells_SG(int tmp[16*2][24]) = 0;

    virtual std::vector<uint32_t> getSmallRJetTOBs() =0;
    virtual std::vector<uint32_t> getLargeRJetTOBs() =0;
    virtual StatusCode NewExecute(int tmp[16*4][17]) = 0;
    virtual StatusCode NewExecute(int tmp[16*4][24]) = 0;
  private:

  };

  inline const InterfaceID& LVL1::IjFEXSim::interfaceID()
  {
    return IID_IjFEXSim;
  }

} // end of namespace

#endif
