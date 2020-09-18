/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           IeFEXegAlgo.h  -
//                              -------------------
//     begin                : 19 06 2020
//     email                :  nicholas.andrew.luongo@cern.ch
//  ***************************************************************************/

#ifndef IeFEXegAlgo_H
#define IeFEXegAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/eFEXegTOB.h"
#include "L1CaloFEXSim/eTowerContainer.h"

namespace LVL1 {
  
/*
Interface definition for eFEXegAlgo
*/

  static const InterfaceID IID_IeFEXegAlgo("LVL1::IeFEXegAlgo", 1, 0);

  class IeFEXegAlgo : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode safetyTest() = 0;
    virtual void setup(int inputTable[3][3]) = 0;

    virtual std::vector<unsigned int> getReta() = 0;
    virtual void getRhad(std::vector<unsigned int> & ) = 0;
    virtual void getWstot(std::vector<unsigned int> & ) = 0;
    virtual void getRealPhi(float & phi) = 0;
    virtual void getRealEta(float & eta) = 0;
    virtual std::unique_ptr<eFEXegTOB> geteFEXegTOB() = 0;
    virtual unsigned int getET() = 0;
    virtual void getWindowET(int layer, int jPhi, int SCID, unsigned int &) = 0;
    virtual bool haveSeed() = 0;
    virtual void getCoreEMTowerET(unsigned int & et) = 0;
    virtual void getCoreHADTowerET(unsigned int & et) = 0;

  private:
    virtual void setSeed() = 0;

  };

  inline const InterfaceID& LVL1::IeFEXegAlgo::interfaceID()
  {
    return IID_IeFEXegAlgo;
  }

} // end of namespace

#endif
