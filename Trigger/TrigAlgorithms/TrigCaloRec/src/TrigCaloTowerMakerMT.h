// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloTowerMakerMT
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    P.A. Delsart
 * CREATED:   August 2006
 *
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGCALOTOWERMAKERMT_H
#define TRIGCALOREC_TRIGCALOTOWERMAKERMT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


class ICaloCellMakerTool; 
class CaloTowerBuilderToolBase;
class CaloTowerContainer;


class TrigCaloTowerMakerMT : public AthAlgorithm {

 public:

  /**  constructor */
  TrigCaloTowerMakerMT(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~TrigCaloTowerMakerMT();

  /** HLT method to initialize */
 StatusCode initialize();

  /** HLT method to finalize */
 StatusCode finalize();

  StatusCode execute();
 private:

  /** Number of eta segments in which we divide the calorimeter */
  unsigned int m_nEtaTowers;

  /** Number of phi segments in which we divide the calorimeter */
  unsigned int m_nPhiTowers;

  /** Eta limits of the region where the towers are built */
  double m_minEta, m_maxEta;
  double m_deta, m_dphi;

  /** Name of the offlinet tools for tower making wrapped-up by TrigCaloTowerMakerMT */
  std::vector<std::string> m_towerMakerNames;

  /** Pointers for offline tower maker tools */
  std::vector<CaloTowerBuilderToolBase*> m_towerMakerPointers;

  /** Pointers to timers */
  //  TrigTimer* m_totalTimer;
  //TrigTimer* m_towerContTimer;
 
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_inputRoiKey{ this,
      "RoIs",         // property name
      "rois",                                                  // default value of StoreGate key
      "input RoI collection name"};

  SG::ReadHandleKey<CaloCellContainer> m_inputCellsKey{ this,
      "Cells",                  // property name
      "cells",                                             // default value of StoreGate key
      "input CaloCellContainer "};

  SG::WriteHandleKey<CaloTowerContainer> m_outputTowerKey{ this,
      "CaloTowers",                  // property name
      "calotowers",                                             // default value of StoreGate key
      "output CaloTowerContainer"};

  SG::WriteHandleKey< INavigable4MomentumCollection> m_caloTowerNav4LinkKey{this,
      "CaloTowersSymLinkName",                  // property name
      "calotowers",                                             // default value of StoreGate key
      "Calo Towers SymLink Name - don't set this"};


  /** Tower Container size (for monitoring purpouses). */
  //  double m_TowerContainerSize;

  /** To help structure Tower container */
  bool m_includeFcal;

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

public:

  //  inline CaloTowerContainer* GetTowerContainer() const {return m_pCaloTowerContainer;}

};
#endif
