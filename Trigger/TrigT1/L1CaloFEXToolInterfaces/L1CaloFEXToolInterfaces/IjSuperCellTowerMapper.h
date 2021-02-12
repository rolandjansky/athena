/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           IjSuperCellTowerMapper.h  -
//                              -------------------
//     begin                : 23 03 2019
//     email                :  jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef IjSuperCellTowerMapper_H
#define IjSuperCellTowerMapper_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/jTower.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace LVL1 {
  
/*
Interface definition for jSuperCellTowerMapper
*/

  static const InterfaceID IID_IjSuperCellTowerMapper("LVL1::IjSuperCellTowerMapper", 1, 0);

  class IjSuperCellTowerMapper : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode AssignSuperCellsToTowers(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw) = 0;
    virtual StatusCode AssignTriggerTowerMapper(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw) = 0;
    
    virtual void reset() = 0;
    
    virtual int FindAndConnectTower(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw,CaloSampling::CaloSample sample,const int region, int layer, const int pos_neg, const int eta_index, const int phi_index, Identifier ID, float et, int prov, bool doPrint,float eta_min, float eta_max, float eta0, float phi_min, float phi_max, float phi0) = 0;
    virtual void ConnectSuperCellToTower(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw, int iETower, Identifier ID, int iCell, float et, int layer, bool doenergysplit) = 0;
    virtual int FindTowerIDForSuperCell(int towereta, int towerphi) = 0;
    virtual void PrintCellSpec(const CaloSampling::CaloSample sample, int layer, const int region, const int eta_index, const int phi_index, const int pos_neg, int iETower, int iCell, int prov, Identifier ID, bool doenergysplit,float eta_min, float eta_max, float eta0, float phi_min, float phi_max, float phi0, bool cellValid = false) = 0;

  private:

  };

  inline const InterfaceID& LVL1::IjSuperCellTowerMapper::interfaceID()
  {
    return IID_IjSuperCellTowerMapper;
  }

} // end of namespace

#endif
