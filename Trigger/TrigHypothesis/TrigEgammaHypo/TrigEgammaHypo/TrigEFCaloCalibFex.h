// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigEFCaloCalibFex.h
 **
 **   Description: Fex algo to apply MVA calibration to EF CaloClusters 
 **
 **   Authors: R. M. White     <ryan.white@cern.ch>
 **
 **   Created: October 2014 
 **
 **************************************************************************/ 

#ifndef TRIG_TRIGEFCALOCALIBFEX_H 
#define TRIG_TRIGEFCALOCALIBFEX_H

// standard stuff
#include <vector>
#include <cmath> 

// general athena stuff
#include "GaudiKernel/IToolSvc.h"

// trigger includes
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloUtils/CaloCellDetPos.h"
class TrigCaloQuality;
class IegammaMVATool;
/**
 * \class TrigEFCaloCalibFex 
 * \brief TrigEFCaloCalibFex is a Trigger Fex Algorithm that retrieves the EF xAODCaloCluster
 * deep copies the clusters, applies MVA calibration and Et selection and attaches new container 
 *
 */

class TrigEFCaloCalibFex : public HLT::FexAlgo  {
  
 public:

  TrigEFCaloCalibFex(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFCaloCalibFex();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  void fillPositionsInCalo(xAOD::CaloCluster* cluster); 
  std::string retrieveCellContName (const HLT::TriggerElement *inputTE );
  TrigCaloQuality*      pTrigCaloQuality;
 private:
  
  
  // to set Accept-All mode 
  bool m_acceptAll;
  bool m_applyMVACalib;
  std::string m_egType;
  std::string m_persKey; // Cluster container key
  xAOD::CaloClusterContainer* m_pCaloClusterContainer; //!<  pointer to CaloClusterContainer 
  
  CaloCellDetPos *m_caloCellDetPos;
  /** Handle to the MVA calibration Tool **/
  ToolHandle<IegammaMVATool>  m_MVACalibTool;  

  // Timers
  TrigTimer* m_totalTimer;
  TrigTimer* m_toolTimer;
  // Monitor collections
  std::vector<float> m_EBE0;
  std::vector<float> m_EBE1;
  std::vector<float> m_EBE2;
  std::vector<float> m_EBE3;
  std::vector<float> m_Eta;
  std::vector<float> m_Phi;
  std::vector<float> m_EtaCalo;
  std::vector<float> m_PhiCalo;
  std::vector<float> m_E;
  std::vector<float> m_ECalib;
  std::vector<float> m_ERes;
};

#endif
