/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGMUONEF_TRIGMUONEFTRACKISOLATIONALGMT_H
#define _TRIGMUONEF_TRIGMUONEFTRACKISOLATIONALGMT_H 1

// general athena stuff 
#include "GaudiKernel/IToolSvc.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"

//Gaudi 
#include "GaudiKernel/ToolHandle.h" 
#include "GaudiKernel/ServiceHandle.h"

// Base class
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/ReadHandleKey.h" 
#include "StoreGate/WriteHandleKey.h"

// Retrieve containers
#include "Particle/TrackParticleContainer.h" 
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"


//#include "AthenaMonitoring/GenericMonitoringTool.h"

class TrigMuonEFTrackIsolationAlgMT : public AthAlgorithm
{
  public :

    /** Constructor **/
    TrigMuonEFTrackIsolationAlgMT( const std::string& name, ISvcLocator* pSvcLocator );
    /** Destructor **/
    ~TrigMuonEFTrackIsolationAlgMT(){};
  
    /** initialize. Called by the Steering. */
    StatusCode initialize();
  
    /** finalize. Called by the Steering. */
    StatusCode finalize();
  
    /** execute execute the combined muon FEX. */
    StatusCode execute();


  private :

    // ID Tracks and EF Muons
//    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_idTrackParticleKey {
//  	this, "IdTrackParticles", "InDetTrigTrackingxAODCnv_Muon_FTF", "Name of ID Track Particle container" };
//  
    //SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_efMuonContainerKey {
    SG::ReadHandleKey<xAOD::MuonContainer> m_efMuonContainerKey {
  	this, "MuonEFContainer", "Muons", "Name of EF Muons container" };
//
//    // FTK Tracks and L2 Muons  
//    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_ftkTrackParticleKey {
//  	this, "FTKTrackParticles", "InDetTrigTrackingxAODCnv_Muon_FTK", "Name of FTK Track Particle container" };
//  
//    SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_l2MuonContainerKey {
//	this, "MuonL2Container", "MuonL2CBInfo", "Name of L2 Muons container" };
//
    // Which setups to run
    Gaudi::Property<int> m_isoType { this, "IsoType", 1, "Which setups to run"};
  
    // Require that EF muons are combined (should move to pass bits)
    Gaudi::Property<bool> m_requireCombined { this, "requireCombinedMuon", false, "Require that EF Muons are combined"};
  
    // Use offline isolation variables
    Gaudi::Property<bool> m_useVarIso { this, "useVarIso", false, "Use offline isolation variables"};
  
    // cone sizes to calculate the isolation
    std::vector<double> m_coneSizes; 

    // Timers
    Gaudi::Property<bool> m_doMyTimimg { this, "doMyTimimg", false, "Require TimerSvc"};
    ServiceHandle<ITrigTimerSvc> m_timerSvc;
    TrigTimer* m_dataPrepTime;
    TrigTimer* m_calcTime;
    TrigTimer* m_dataOutputTime;

};


#endif
