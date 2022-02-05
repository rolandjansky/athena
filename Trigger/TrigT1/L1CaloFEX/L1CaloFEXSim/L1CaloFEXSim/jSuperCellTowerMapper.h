/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JSUPERCELLTOWERMAPPER_H
#define JSUPERCELLTOWERMAPPER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjSuperCellTowerMapper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/jTower.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class CaloIdManager;

namespace LVL1 {


class jSuperCellTowerMapper: public AthAlgTool, virtual public IjSuperCellTowerMapper
{
 public:
  jSuperCellTowerMapper(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~jSuperCellTowerMapper();

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() override;
  
  virtual StatusCode AssignSuperCellsToTowers(std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw) override;
  virtual StatusCode AssignTriggerTowerMapper(std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw) override;
  virtual StatusCode AssignPileupAndNoiseValues (std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw,TH1F *jTowerArea_hist, TH1I *Firmware2BitwiseID,TH1I *BinLayer,TH1F *EtaCoords,TH1F *PhiCoords) override;
  
  virtual void reset() override;

 private:

  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_triggerTowerCollectionSGKey {this, "xODTriggerTowers", "xAODTriggerTowers", "xAODTriggerTowers"};

  virtual int FindAndConnectTower(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw,CaloSampling::CaloSample sample,const int region, int layer, const int pos_neg, const int eta_index, const int phi_index, Identifier ID, float et, int prov, bool doPrint,float eta_min, float eta_max, float eta0, float phi_min, float phi_max, float phi0) override;
  virtual void ConnectSuperCellToTower(std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw, int iETower, Identifier ID, int iCell, float et, int layer) override;
  virtual int FindTowerIDForSuperCell(int towereta, int towerphi) override;
  virtual void PrintCellSpec(const CaloSampling::CaloSample sample, int layer, const int region, const int eta_index, const int phi_index, const int pos_neg, int iETower, int iCell, int prov, Identifier ID, bool doenergysplit,float eta_min, float eta_max, float eta0, float phi_min, float phi_max, float phi0,bool cellValid=true) override;
  

};

} // end of LVL1 namespace
#endif
