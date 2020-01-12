/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

// Retrieve containers
#include "Particle/TrackParticleContainer.h" 
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuonContainer.h"

#include "TrigMuonToolInterfaces/IMuonEFTrackIsolationTool.h" 
#include "AthenaMonitoringKernel/Monitored.h"


//#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

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

    /// Tool handle for isolation tool
    ToolHandle<IMuonEFTrackIsolationTool> m_onlineEfIsoTool {
	this, "OnlineIsolationTool", "TrigMuonEFTrackIsolationTool/TrigMuonEFTrackIsolationTool", "Select online muon isolation tool you want to use"};

    // ID Tracks and EF Muons
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey {
  	this, "IdTrackParticles", "InDetTrigTrackingxAODCnv_Muon_FTF", "Name of ID (or FTK) Track Particle container" };
  
    SG::ReadHandleKey<xAOD::MuonContainer> m_efMuonContainerKey {
  	this, "MuonEFContainer", "Muons", "Name of EF Muons container" };

    // L2 Muons  
    SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_l2MuonContainerKey {
	this, "MuonL2Container", "MuonL2CBInfo", "Name of L2 Muons container" };

    SG::WriteHandleKey<xAOD::MuonContainer> m_muonContainerKey {
	this, "MuonContName", "MuonEFInfo", "Name of output objects for EF" };

    SG::WriteHandleKey<xAOD::L2IsoMuonContainer> m_l2MuonIsoContainerKey {
        this, "L2IsoMuonContName", "MuonL2ISInfo", "Name of output objects for L2" };


    // Which setups to run
    Gaudi::Property<int> m_isoType { this, "IsoType", 1, "Which setups to run"};
  
    // Require that EF muons are combined
    Gaudi::Property<bool> m_requireCombined { this, "requireCombinedMuon", true, "Require that EF Muons are combined"};
  
    // Use offline isolation variables
    Gaudi::Property<bool> m_useVarIso { this, "useVarIso", true, "Use offline isolation variables"};
  
    // cone sizes to calculate the isolation
    std::vector<double> m_coneSizes; 

    // Monitoring tool
    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

};


#endif
