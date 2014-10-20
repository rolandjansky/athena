/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFCaloHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   P. Conde Muino
 * CREATED:  Nov 25, 2004
 * MODIFIED: V. Perez Reale   migration to new Steering 1/12/06
 *
 *********************************************************************/
#ifndef TRIGEFCALOHYPO_H
#define TRIGEFCALOHYPO_H

#include <string>
#include <cmath> 
//#include <map.h>

// athena stuff stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/HypoAlgo.h"


//#include <string
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IegammaShowerShape.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
class StoreGateSvc;
class TriggerElement;



/**
 * \class TrigEFCaloHypo
 * \brief TrigEFCaloHypo is a Trigger Hypo Algorithm that retrieves the EF CaloCluster
 * container created by TrigCaloRec
 * Apply calorimeter selction cuts like ET
 *
 */


class TrigEFCaloHypo : public HLT::HypoAlgo {

 public:

  TrigEFCaloHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFCaloHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------
  bool m_UseShowerShapeTool;
  bool m_acceptAll;
  // Cuts to be applied:

  double     m_EtCut;
  double     m_deltaPhiCut;
  double     m_deltaEtaCut;
  double     m_energySecondSampling;
  double     m_isEM;
  double     m_etCalibFactor;


  // Switch on Monitoring:
  ToolHandle<IegammaShowerShape> m_ShowerShapeTool;
  std::string m_SelectorToolName;
  ToolHandle<IAsgElectronIsEMSelector> m_SelectorTool;
  
  bool m_doMonitoring;
  
  // Histograms:

 /*  IHistogram1D* m_efOrigEt; */
/*   IHistogram1D* m_efDeltaPhi; */
/*   IHistogram1D* m_efDeltaEta; */
/*   IHistogram1D* m_efege277; */
/*   IHistogram1D* m_efisEM; */


  // path for the histograms:

  std::string m_path;

  // Histogram Service:
  IHistogramSvc* m_histsvc;


 // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
  TrigTimer* m_totalTimer;

};
#endif

