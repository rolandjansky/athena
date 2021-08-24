/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gSuperCellTowerMapper - supercells and Tile tower assignment to gTowers
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************


#ifndef GSUPERCELLTOWERMAPPER_H
#define GSUPERCELLTOWERMAPPER_H

// STL
#include <string>
#include "L1CaloFEXToolInterfaces/IgSuperCellTowerMapper.h"


class CaloIdManager;

namespace LVL1 {


class gSuperCellTowerMapper: public AthAlgTool, virtual public IgSuperCellTowerMapper
{
 public:
  gSuperCellTowerMapper(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~gSuperCellTowerMapper();

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() override;

  virtual StatusCode AssignSuperCellsToTowers(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw) const override;
  virtual StatusCode AssignTriggerTowerMapper(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw) const override;

  virtual void reset() override;

 private:

  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_triggerTowerCollectionSGKey {this, "xODTriggerTowers", "xAODTriggerTowers", "xAODTriggerTowers"};

  virtual int FindAndConnectTower(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw,CaloSampling::CaloSample sample,
                                  const int region, const int pos_neg, const int eta_index, const int phi_index, Identifier ID,
                                  float et, int prov, bool doPrint) const override;

  virtual void ConnectSuperCellToTower(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw,
                                       int iGTower, Identifier ID, float et) const override;

  virtual int FindTowerIDForSuperCell(int towereta, int towerphi, int nphi) const override;

  virtual void PrintCellSpec(const CaloSampling::CaloSample sample, const int region,
                               const int eta_index, const int phi_index, const int pos_neg,
                               int towereta, int towerphi, int nphi, int towerID_Modifier,
                               int iGTower, int prov, Identifier ID) const override;
};


} // end of LVL1 namespace
#endif
