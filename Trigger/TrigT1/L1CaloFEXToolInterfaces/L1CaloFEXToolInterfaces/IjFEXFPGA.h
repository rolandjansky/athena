/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXFPGA.h  -
//                              -------------------
//     begin                : 23 03 2019
//     email                :  jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef IjFEXFPGA_H
#define IjFEXFPGA_H


#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jTower.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

namespace LVL1 {
  
/*
Interface definition for jFEXFPGA
*/

  static const InterfaceID IID_IjFEXFPGA("LVL1::IjFEXFPGA", 1, 0);

  class IjFEXFPGA : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode init(int id, int efexid) = 0;

    virtual StatusCode execute() = 0;

    virtual void reset() = 0;

    virtual int ID() = 0;
    
    virtual uint32_t formSmallRJetTOB(int &, int &) =0;
    virtual std::vector<uint32_t> getjTOBs() = 0;

    virtual void SetTowersAndCells_SG(int [][17]) = 0;
    virtual void SetTowersAndCells_SG(int [][24]) = 0;

  private:

  };

  inline const InterfaceID& LVL1::IjFEXFPGA::interfaceID()
  {
    return IID_IjFEXFPGA;
  }

} // end of namespace

#endif
