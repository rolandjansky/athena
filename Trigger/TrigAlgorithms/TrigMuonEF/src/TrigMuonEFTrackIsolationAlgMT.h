/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGMUONEF_TRIGMUONEFTRACKISOLATIONALGMT_H
#define _TRIGMUONEF_TRIGMUONEFTRACKISOLATIONALGMT_H 1

// Base class
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/ReadHandleKey.h" 
#include "StoreGate/WriteHandleKey.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

// Retrieve containers
#include "Particle/TrackParticleContainer.h" 
#include "xAODMuon/MuonContainer.h"

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
  	this, "IdTrackParticles", "InDetTrigTrackingxAODCnv_Muon_FTF", "Name of ID Track Particle container" };
  
    SG::ReadHandleKey<xAOD::MuonContainer> m_efMuonContainerKey {
  	this, "MuonEFContainer", "Muons", "Name of EF Muons container" };

    SG::WriteHandleKey<xAOD::MuonContainer> m_muonContainerKey {
	this, "MuonContName", "MuonEFInfo", "Name of output objects for EF" };

    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonIso20Key {
	this, "ptcone02Name", "Muons.ptcone02", "Isolation for ptcone 0.2" };

    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonIso30Key {
	this, "ptcone03Name", "Muons.ptcone03", "Isolation for ptcone 0.3" };


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
