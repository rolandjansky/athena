/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloJet.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Kyle Cranmer
//
// Description: Level2 Jet algorithm.  Basically copied from L2 EM and
// Tau algs with some cleanup.
// ********************************************************************

#ifndef TRIGT2CALOJET_T2CALOJET_H
#define TRIGT2CALOJET_T2CALOJET_H

#include <string>
#include "TrigT2CaloCommon/T2CaloBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigCaloEvent/TrigCaloCluster.h"

class StoreGateSvc;
//class TriggerElement;
//class TrigRoiDescriptor;
class T2CaloJetBaseTool;
class TrigT2Jet;

class T2CaloJet: public T2CaloBase {


 public:
  T2CaloJet(const std::string & name, ISvcLocator* pSvcLocator);
  ~T2CaloJet();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE);


  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

 private:

  //  double m_etamin, m_etamax, m_phimin, m_phimax;
  //  TriggerElement* m_inputTE ;
  //  TrigRoiDescriptor* m_roiDescriptor;
  TrigT2Jet* m_jet;
  TrigCaloCluster *m_layerInfo;

  //std::vector<T2CaloJetBaseTool*> m_tools;
  ToolHandleArray< T2CaloJetBaseTool > m_tools;

  // Properties:
  std::string               m_jetOutputKey;
  std::string               m_clusterOutputKey;
  //std::vector<std::string>  m_toolNames;
  bool		      m_clearJetGrid;
  bool                m_fillLayerInfo;

  // Monitored Variables
  double              m_dR;
  double              m_e;     // total energy of the jet after calibration
  double              m_et;
  double              m_ehad0; // hadronic energy before calibration
  double              m_eem0;  // electromagnetic energy before calibration
  
  double              m_ethad0; // transverse energy : hadronic component before calibration
  double              m_etem0;  // transverse energy : electromagnetic component before calibration
  
  double              m_eta;
  double              m_phi;

  double              m_etaIDWidth;
  double              m_phiIDWidth;

  unsigned int m_conversionError;
  unsigned int m_algorithmError;



 protected:

  std::vector<TrigTimer*> m_timer;
  const TrigTimer *m_timeLArUnpck;
  const TrigTimer *m_timeTileUnpck;
  const TrigTimer *m_timeHECUnpck;

 public:

  inline TrigT2Jet* GetTrigT2Jet() const {return m_jet;}
//inline TrigCaloCluster* GetTrigCaloCluster() const {return m_layerInfo;}

};

#endif
