/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IgSuperCellTowerMapper_H
#define IgSuperCellTowerMapper_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/gTower.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace LVL1 {

/*
Interface definition for gSuperCellTowerMapper
*/

  static const InterfaceID IID_IgSuperCellTowerMapper("LVL1::IgSuperCellTowerMapper", 1, 0);

  class IgSuperCellTowerMapper : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual StatusCode AssignSuperCellsToTowers(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw) const = 0;
    virtual StatusCode AssignTriggerTowerMapper(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw) const = 0;

    virtual void reset() = 0;

    virtual int FindAndConnectTower(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw,CaloSampling::CaloSample sample,const int region, const int pos_neg, const int eta_index, const int phi_index, Identifier ID, float et, int prov, bool doPrint) const = 0;
    virtual void ConnectSuperCellToTower(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw, int iGTower, Identifier ID, float et) const = 0;
    virtual int FindTowerIDForSuperCell(int towereta, int towerphi, int nphi) const = 0;

    virtual void PrintCellSpec(const CaloSampling::CaloSample sample, const int region,
                               const int eta_index, const int phi_index, const int pos_neg,
                               int towereta, int towerphi, int nphi, int towerID_Modifier,
                               int iGTower, int prov, Identifier ID) const = 0;
  private:

  };

  inline const InterfaceID& LVL1::IgSuperCellTowerMapper::interfaceID()
  {
    return IID_IgSuperCellTowerMapper;
  }

} // end of namespace

#endif
