/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXFPGA.h  -
//                              -------------------
//     begin                : 23 03 2019
//     email                :  jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef IeFEXFPGA_H
#define IeFEXFPGA_H


#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/eTower.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

namespace LVL1 {
  
/*
Interface definition for eFEXFPGA
*/

  static const InterfaceID IID_IeFEXFPGA("LVL1::IeFEXFPGA", 1, 0);

  class IeFEXFPGA : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode init(int id, int efexid) = 0;

    virtual StatusCode execute() = 0;

    virtual void reset() = 0;

    virtual int ID() = 0;

    virtual void SetTowersAndCells_SG(int [][6]) = 0;

  private:

  };

  inline const InterfaceID& LVL1::IeFEXFPGA::interfaceID()
  {
    return IID_IeFEXFPGA;
  }

} // end of namespace

#endif
