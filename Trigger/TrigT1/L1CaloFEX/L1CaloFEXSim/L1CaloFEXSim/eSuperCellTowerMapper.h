/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ESUPERCELLTOWERMAPPER_H
#define ESUPERCELLTOWERMAPPER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IeSuperCellTowerMapper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/eTower.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class CaloIdManager;

namespace LVL1 {


class eSuperCellTowerMapper: public AthAlgTool, virtual public IeSuperCellTowerMapper
{
 public:
  eSuperCellTowerMapper(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~eSuperCellTowerMapper();

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() ;
  
  virtual StatusCode AssignSuperCellsToTowers(/*eTowerContainer**/std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw) override;
  virtual StatusCode AssignTriggerTowerMapper(/*eTowerContainer**/std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw) override;
  
  virtual void reset() override;

 private:

  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_triggerTowerCollectionSGKey {this, "xODTriggerTowers", "xAODTriggerTowers", "xAODTriggerTowers"};

  virtual int FindAndConnectTower(/*eTowerContainer**/std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw,CaloSampling::CaloSample sample,const int region, int layer, const int pos_neg, const int eta_index, const int phi_index, Identifier ID, float et, int prov, bool doPrint) override;
  virtual int ConnectSuperCellToTower(/*eTowerContainer**/std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw, int iETower, Identifier ID, int iCell, float et, int layer, bool doenergysplit) override;
  virtual int FindTowerIDForSuperCell(int towereta, int towerphi) override;
  virtual void PrintCellSpec(const CaloSampling::CaloSample sample, int layer, const int region, const int eta_index, const int phi_index, const int pos_neg, int iETower, int iCell, int prov, Identifier ID, bool doenergysplit) override;

};

} // end of LVL1 namespace
#endif
