// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloTowerMaker
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    P.A. Delsart
 * CREATED:   August 2006
 *
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGCALOTOWERMAKER_H
#define TRIGCALOREC_TRIGCALOTOWERMAKER_H

#include "TrigInterfaces/FexAlgo.h"
#include "CaloEvent/CaloTowerContainer.h"


class IAlgToolEFCalo;
class ICaloCellMakerTool; 
class CaloTowerBuilderToolBase;
class CaloTowerContainer;


class TrigCaloTowerMaker : public HLT::FexAlgo {

 public:

  /**  constructor */
  TrigCaloTowerMaker(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~TrigCaloTowerMaker();

  /** HLT method to initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method to execute FEX algo on a given TE. 
   input is the Triger Element (TE) from the previous algorithm in
   the configuration chain, the output is the result from the
   TrigCaloTowerMaker execution. **/
  HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );
 private:

  /** Number of eta segments in which we divide the calorimeter */
  unsigned int m_nEtaTowers;

  /** Number of phi segments in which we divide the calorimeter */
  unsigned int m_nPhiTowers;

  /** Eta limits of the region where the towers are built */
  double m_minEta, m_maxEta;
  double m_deta, m_dphi;

  /** Name of the offlinet tools for tower making wrapped-up by TrigCaloTowerMaker */
  std::vector<std::string> m_towerMakerNames;

  /** Pointers for offline tower maker tools */
  std::vector<CaloTowerBuilderToolBase*> m_towerMakerPointers;

  /** Pointers to timers */
  TrigTimer* m_totalTimer;
  TrigTimer* m_towerContTimer;
 
  /** Produced Calorimeter Tower Container. */
  CaloTowerContainer* m_pCaloTowerContainer;

  /** Tower Container size (for monitoring purpouses). */
  double m_TowerContainerSize;

  /** To help structure Tower container */
  bool m_includeFcal;

public:

  inline CaloTowerContainer* GetTowerContainer() const {return m_pCaloTowerContainer;}

};
#endif
