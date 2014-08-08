// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloTopoTowerAlgorithm
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGCALOTOPOTOWERALGORITHM_H
#define TRIGCALOREC_TRIGCALOTOPOTOWERALGORITHM_H

#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloTopoTowerBuilderToolBase.h"


class TrigCaloTopoTowerAlgorithm : public HLT::FexAlgo {

 public:

  TrigCaloTopoTowerAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigCaloTopoTowerAlgorithm();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );



protected:

  // Container name strings                                                                                                             
  std::string m_newTowerContainerName;  // output tower name           

  // Selection criteria                                                                                                                 
  double m_minimumCellEnergy;
  double m_minimumClusterEnergy;

  // Noise tool stuff                                                                                                                   
  float m_noiseSigma;
  bool m_useNoiseTool;
  bool m_usePileUpNoise;
  float m_cellESignificanceThreshold;

  // List of calorimeters from which to use cells                                                                                       
  std::vector<std::string> m_includedCalos;
  bool   m_useCellWeights;

  std::vector<CaloCell_ID::SUBCALO> m_caloIndices;
  bool m_caloSelection;

  // tool collection                                                                                                              
  ToolHandleArray<ICaloTopoTowerBuilderToolBase> m_ptools;


};
#endif
