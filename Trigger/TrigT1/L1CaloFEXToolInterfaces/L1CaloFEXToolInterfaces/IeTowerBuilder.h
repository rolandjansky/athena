/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           IeTowerBuilder.h  -
//                              -------------------
//     begin                : 23 03 2019
//     email                :  jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef IeTowerBuilder_H
#define IeTowerBuilder_H

#include "GaudiKernel/IAlgTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"

namespace LVL1 {
  
/*
Interface definition for eTowerBuilder
*/

  static const InterfaceID IID_IeTowerBuilder("LVL1::IeTowerBuilder", 1, 0);

  class IeTowerBuilder : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual void BuildEMBeTowers(std::unique_ptr<eTowerContainer> & eTowerContainer) = 0;
    virtual void BuildTRANSeTowers(std::unique_ptr<eTowerContainer> & eTowerContainer) = 0;
    virtual void BuildEMEeTowers(std::unique_ptr<eTowerContainer> & eTowerContainer) = 0 ;
    virtual void BuildHECeTowers(std::unique_ptr<eTowerContainer> & eTowerContainer) = 0;
    virtual void BuildAllTowers(std::unique_ptr<eTowerContainer> & eTowerContainer) = 0;
    virtual void BuildSingleTower(std::unique_ptr<eTowerContainer> & eTowerContainer,float eta, float phi, float keybase, int posneg) = 0;
    
    virtual void init(std::unique_ptr<eTowerContainer> & eTowerContainer) = 0;
    virtual void execute(std::unique_ptr<eTowerContainer> & eTowerContainer) = 0;
    virtual void reset() = 0;


  private:

  };

  inline const InterfaceID& LVL1::IeTowerBuilder::interfaceID()
  {
    return IID_IeTowerBuilder;
  }

} // end of namespace

#endif
