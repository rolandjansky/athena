/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           IeFEXFPGA.h  -
//                              -------------------
//     begin                : 23 03 2019
//     email                :  jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef IeFEXFPGA_H
#define IeFEXFPGA_H


#include "GaudiKernel/IAlgTool.h"
#include "xAODTrigger/eFexEMRoI.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"

namespace LVL1 {
  
/*
Interface definition for eFEXFPGA
*/

  static const InterfaceID IID_IeFEXFPGA("LVL1::IeFEXFPGA", 1, 0);

  class IeFEXFPGA : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode init(int id, int efexid) = 0;

    virtual StatusCode execute(eFEXOutputCollection* inputOutputCollection) = 0;

    virtual void reset() = 0;

    virtual int getID() = 0;

    virtual void SetTowersAndCells_SG(int [][6]) = 0;
    
    virtual void SetIsoWP(std::vector<unsigned int> &, std::vector<unsigned int> &, unsigned int &, unsigned int &) = 0;

    virtual std::vector<uint32_t> getEmTOBs() = 0; 

    virtual std::vector<uint32_t> getTauTOBs() = 0; 

    // static virtual bool etSort(uint32_t &, uint32_t &) = 0;

  private:

  };

  inline const InterfaceID& LVL1::IeFEXFPGA::interfaceID()
  {
    return IID_IeFEXFPGA;
  }

} // end of namespace

#endif

