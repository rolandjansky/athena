/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     muIso.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuIso
// 
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  LVL2 Muon Isolation FEX Algorithm: V6.0
// ********************************************************************

#ifndef TRIGMUISO_MUISO_H 
#define TRIGMUISO_MUISO_H

#include <string>
#include "GaudiKernel/Algorithm.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

//adding a part of DataHandle for AthenaMT
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"    
#include "StoreGate/WriteHandleKey.h"  
#include "AthenaMonitoring/GenericMonitoringTool.h"  

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

class StoreGateSvc;
class TrigTimer;

/** Main LVL2 Algorithm. Sided by a xAOD::L2CombinedMuon, calculate trackID isolation 
    around the muon direction and fill a xAOD::L2IsoMuonContainer. */
class muIso: public HLT::FexAlgo {

 public:
  /** Constructor */
  muIso(const std::string & name, ISvcLocator* pSvcLocator);
  /** Destructor */
  ~muIso();

  /** hltExecute execute the muon isolation FEX. */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  /** hltInitialize. Called by the Steering. */
  HLT::ErrorCode hltInitialize();
  /** hltFinalize. Called by the Steering. */
  HLT::ErrorCode hltFinalize();

  /** recordFeature. Record on the event record the output xAOD::L2IsoMuonContainer*) */
  HLT::ErrorCode muIsoSeed(HLT::TriggerElement* , xAOD::L2IsoMuonContainer*);


  /** StatusCode execute(), main code of the algorithm for athenaMT **/
  StatusCode execute();

  StatusCode findIsolation( const xAOD::L2CombinedMuonContainer& muonColl, 
                            const xAOD::TrackParticleContainer& idTrackParticles,
                            xAOD::L2IsoMuonContainer& muonISColl );
 private:

  /** Pointer to Storegate. */
  //ServiceHandle<StoreGateSvc>  m_pStoreGate;
  StoreGateSvc*  m_pStoreGate;

  /** Timer service Handle. */
  ServiceHandle<ITrigTimerSvc> m_pTimerService;

  /** ID Track collection used for matching */
  Gaudi::Property< std::string >  m_ID_algo_to_use {
	this, "IDalgo", "InDetTrigTrackingxAODCnv_Muon_FTF", "ID Track collection used for matching"};

  /** Cone radius for ID based isolation. */
  Gaudi::Property< double >  m_RID {
	this, "RID", 0.4, "Cone radius for ID based isolation"};
  /** Min Pt to select the ID track for matching */
  Gaudi::Property< double >  m_PtMinTrk {
	this, "MinPtTRK", 1.0, "Min Pt to select the ID track for matching"};
  /** Max abs(eta) to select the ID track for matching */
  Gaudi::Property< double >  m_EtaMaxTrk {
	this, "MaxAbsEtaTRK", 2.5, "Max abs(eta) to select the ID track for matching"};
  /** Max abs(Z_ID - Z_muon) for ID tracks to enter the pt cone sum */
  Gaudi::Property< double >  m_DzetaMax {
	this, "MaxDzetaIDMuon", 15.0, "Max abs(Z_ID - Z_muon) for ID tracks to enter the pt cone sum"};

  /** Isolation cut used for Monitoring (ID) */
  Gaudi::Property< double >  m_GlobIsoCut_ID {
	this, "MonitoringGlobalIsoCut_ID", 0.2, "Isolation cut used for Monitoring (ID)"};

  /** Monitored quantities */
  int    m_ErrorFlagMI;
  float  m_ptFL;
  float  m_Sumpt;
  float  m_IDiso;
  float  m_MuPt;
  float  m_MuEta;
  float  m_MuPhi;
  float  m_NTRK;
  float  m_isIsolated;

  /** DataHandle for athenaMT **/
  SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_muonCBKey {
	this, "MuonL2CBInfoName", "MuonL2CBInfo", "Name of the input data produced by muCombMT"};

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_idTrackParticlesKey {
	this, "TrackParticlesName", "InDetTrigTrackingxAODCnv_Muon_FTF", "Name of the input data produced by TrigFastTrackFinder_Muon"};

  SG::WriteHandleKey<xAOD::L2IsoMuonContainer> m_muonISCollKey {
	this, "MuonL2ISInfoName", "MuonL2ISInfo", "Name of the output data"};

  // Monitor system 
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   

};
#endif
