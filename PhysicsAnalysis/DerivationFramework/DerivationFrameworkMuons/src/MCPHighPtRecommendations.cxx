/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MCPHighPtRecommendations.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Implements the MCP 2012 recommendations for hight pt muons
// Implemented for NTUP-to-NTUP

#include "DerivationFrameworkMuons/MCPHighPtRecommendations.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::MCPHighPtRecommendations::MCPHighPtRecommendations( const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
AthAlgTool(t,n,p),
m_ipD03st(0.2),
m_ipZ03st(1.0),
m_trkIso3st(0.05),
m_ipD02st(0.2),
m_ipZ02st(1.0),
m_eta2st(1.05),
m_trkIso2st(0.05),
m_sgKey3st(""),
m_sgKey2st(""),
m_collName("muons")
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("IPd03Stations",m_ipD03st);
    declareProperty("IPz03Stations",m_ipZ03st);
    declareProperty("Isolation3Stations",m_trkIso3st);
    declareProperty("IPd02Stations",m_ipD02st);
    declareProperty("IPz02Stations",m_ipZ02st);
    declareProperty("Eta2Stations", m_eta2st);
    declareProperty("Isolation2Stations",m_trkIso2st);
    declareProperty("StoreGateEntryName3Stations",m_sgKey3st);
    declareProperty("StoreGateEntryName2Stations",m_sgKey2st);
    declareProperty("CollectionNTUPName", m_collName);
}

// Destructor
DerivationFramework::MCPHighPtRecommendations::~MCPHighPtRecommendations() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::MCPHighPtRecommendations::initialize()
{
    if (m_sgKey3st=="" && m_sgKey2st=="") {
	ATH_MSG_ERROR("No SG name provided either for 2- or 3- station selection");
	return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("initialize() ...");
    return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::MCPHighPtRecommendations::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::MCPHighPtRecommendations::addBranches() const
{
    
    // Retrieve data
    CHECK(retrieveFromStoreGate());
    
    // Loop over muons, count up and set decision
    unsigned int nMu = m_mu_isCombinedMuon->size();
    std::vector<int> *passHighPt3St = new std::vector<int>();
    std::vector<int> *passHighPt2St = new std::vector<int>();
    for (unsigned int muItr=0; muItr < nMu; ++muItr) {
        
        float mu_sig_diff_qoverp = getSigDiffQOverP(muItr);
 
        // THREE STATIONS
        bool muonPassesBasic3St = muonPassesBasicSelection(muItr,
                                                            m_ipD03st,
                                                            m_ipZ03st,
                                                            m_trkIso3st);
        bool muonPasses3StHits = muonPasses3StHitReqs(muItr,mu_sig_diff_qoverp);
        bool muonPasses3stations = muonPassesBasic3St && muonPasses3StHits;
        if (muonPasses3stations) {passHighPt3St->push_back(1);}
        else {passHighPt3St->push_back(0);}
        
        // TWO STATIONS
        bool muonPassesBasic2St = muonPassesBasicSelection(muItr,
                                                            m_ipD02st,
                                                            m_ipZ02st,
                                                            m_trkIso2st);
        bool muonPasses2StHits = muonPasses2StHitReqs(muItr,muonPasses3stations,mu_sig_diff_qoverp);
        bool muonPasses2stations = muonPassesBasic2St && muonPasses2StHits;
        if (muonPasses2stations) {passHighPt2St->push_back(1);}
        else {passHighPt2St->push_back(0);}
        
    }
    
    // Write decision to SG for access by downstream algs
    if (m_sgKey3st!="") {
	    if (evtStore()->contains<std::vector<int> >(m_sgKey3st)) {
	        ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgKey3st << " which already exists. Please use a different key");
	        return StatusCode::FAILURE;
	    }
	    CHECK(evtStore()->record(passHighPt3St, m_sgKey3st));
    }
    if (m_sgKey2st!="") {
	    if (evtStore()->contains<std::vector<int> >(m_sgKey2st)) {
	        ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgKey2st << " which already exists. Please use a different key");
	        return StatusCode::FAILURE;
	    }
	    CHECK(evtStore()->record(passHighPt2St, m_sgKey2st));
    } 
    return StatusCode::SUCCESS;
    
}

// Basic selections for high pt muons (no MS hit requirements imposed here)
bool DerivationFramework::MCPHighPtRecommendations::muonPassesBasicSelection(unsigned int counter,
                                                                              float d0Cut,
                                                                              float z0Cut,
                                                                              float isolation
                                                                            ) const {
    bool isCombined = (*m_mu_isCombinedMuon)[counter];
    float d0 = (*m_mu_id_d0_exPV)[counter];
    float z0 = (*m_mu_id_z0_exPV)[counter];
    float muPt = (*m_mu_pt)[counter];
    float sumPt = (*m_mu_ptcone30)[counter];
    
    bool itemPassesSelection = ( isCombined &&
                                (fabs(d0) < d0Cut) &&
                                (fabs(z0) < z0Cut) &&
                                (sumPt/muPt < isolation));
    
    return itemPassesSelection;
    
}

// MS hit requirements for three stations
// Code: Simon Viel & James Coggeshall
bool DerivationFramework::MCPHighPtRecommendations::muonPasses3StHitReqs(unsigned int counter, float mu_sig_diff_qoverp) const {
    
    int MuonPhiHitsCut  = 1;
    int MuonPhiLayerCut = 2;
    int MuonMDTHitsCut  = 3;
    int MuonCSCUnspoiledEtaHitsCut = 2;
    float MuonMomDiffSigCut   = 5.0;  //sigma
    
    //int     mu_nStations(0);
    int     mu_is3StBarrel(0);
    int     mu_is3StEndcap(0);
    int     mu_is3StCSC(0);
    int     mu_is3StEE(0);
    int     mu_is3St(0);
    
    // Count nStations
    /*mu_nStations =
     ( (*m_mu_nMDTBIHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTBMHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTBOHits)[counter] >= MuonMDTHitsCut) +
     (( (*m_mu_nMDTEIHits)[counter] >= MuonMDTHitsCut) || ( (*m_mu_nCSCEtaHits)[counter] >= MuonMDTHitsCut) ) +
     ( (*m_mu_nMDTEMHits)[counter] >= MuonMDTHitsCut) +
     (( (*m_mu_nMDTEOHits)[counter] >= MuonMDTHitsCut) || ( (*m_mu_nMDTEEHits)[counter] >= MuonMDTHitsCut) );
     */
    
    mu_is3StBarrel =
    (fabs(mu_sig_diff_qoverp) < MuonMomDiffSigCut) &&  
    (( (*m_mu_nRPCLayer1PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nRPCLayer2PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nRPCLayer3PhiHits)[counter] >= MuonPhiHitsCut) >= MuonPhiLayerCut) &&
    (( (*m_mu_nMDTBIHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTBMHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTBOHits)[counter] >= MuonMDTHitsCut) == 3) &&
    ( (*m_mu_nMDTBEEHits)[counter] == 0) &&
    ( (*m_mu_nMDTBIS78Hits)[counter] == 0);
    
    mu_is3StEndcap =
    (fabs(mu_sig_diff_qoverp) < MuonMomDiffSigCut) &&
    (( (*m_mu_nTGCLayer1PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer2PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer3PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer4PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nCSCPhiHits)[counter]   >= MuonPhiHitsCut) >= MuonPhiLayerCut) &&
    (( (*m_mu_nMDTEIHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTEMHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTEOHits)[counter] >= MuonMDTHitsCut) == 3) &&
    ( (*m_mu_nMDTBEEHits)[counter] == 0) &&
    ( (*m_mu_nMDTBIS78Hits)[counter] == 0);
    
    mu_is3StCSC =
    !mu_is3StEndcap &&
    (fabs(mu_sig_diff_qoverp) < MuonMomDiffSigCut) &&
    (( (*m_mu_nTGCLayer1PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer2PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer3PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer4PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nCSCPhiHits)[counter]   >= MuonPhiHitsCut) >= MuonPhiLayerCut) &&
    (( (*m_mu_nCSCUnspoiledEtaHits)[counter] >= MuonCSCUnspoiledEtaHitsCut) +
     ( (*m_mu_nMDTEMHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTEOHits)[counter] >= MuonMDTHitsCut) == 3) &&
    ( (*m_mu_nMDTBEEHits)[counter] == 0) &&
    ( (*m_mu_nMDTBIS78Hits)[counter] == 0);
    
    mu_is3StEE =
    !mu_is3StEndcap &&
    (fabs(mu_sig_diff_qoverp) < MuonMomDiffSigCut) &&
    (( (*m_mu_nTGCLayer1PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer2PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer3PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nTGCLayer4PhiHits)[counter] >= MuonPhiHitsCut) +
     ( (*m_mu_nCSCPhiHits)[counter]   >= MuonPhiHitsCut) >= MuonPhiLayerCut) &&
    (( (*m_mu_nMDTEIHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTEEHits)[counter] >= MuonMDTHitsCut) +
     ( (*m_mu_nMDTEMHits)[counter] >= MuonMDTHitsCut) == 3) &&
    ( (*m_mu_nMDTBEEHits)[counter] == 0) &&
    ( (*m_mu_nMDTBIS78Hits)[counter] == 0);
    
    mu_is3St = mu_is3StBarrel || mu_is3StEndcap || mu_is3StCSC || mu_is3StEE;
    
    return(mu_is3St);
    
}

// MS hit requirements for two stations
// Code: Simon Viel & James Coggeshall
bool DerivationFramework::MCPHighPtRecommendations::muonPasses2StHitReqs(unsigned int counter, bool mu_is3St, float mu_sig_diff_qoverp) const {
    
    // Proposed 2012 2-station selection
    int     mu_is2StBarrel(0);
    //int     mu_is2StEndcap(0);
    int     mu_is2St(0);
    
    int MuonPhiHitsCut2  = 1;
    int MuonPhiLayerCut2 = 1;
    int MuonMDTHitsCut2  = 5;
    int MuonMomDiffSigCut2  = 3;  //sigma
    
    float mu_phiSector = getPhiSector( (*m_mu_phi)[counter]);
    
    mu_is2StBarrel =
    !mu_is3St &&
    (fabs( mu_sig_diff_qoverp) < MuonMomDiffSigCut2) &&
    (( (*m_mu_nRPCLayer1PhiHits)[counter] >= MuonPhiHitsCut2) +
     ( (*m_mu_nRPCLayer2PhiHits)[counter] >= MuonPhiHitsCut2) +
     ( (*m_mu_nRPCLayer3PhiHits)[counter] >= MuonPhiHitsCut2) >= MuonPhiLayerCut2) &&
    (( (*m_mu_nMDTBIHits)[counter] >= MuonMDTHitsCut2) +
     ( (*m_mu_nMDTBOHits)[counter] >= MuonMDTHitsCut2) == 2) &&
    ( (*m_mu_nMDTEIHits)[counter] == 0) &&
    ( (*m_mu_nMDTEMHits)[counter] == 0) &&
    ( (*m_mu_nMDTEOHits)[counter] == 0) &&
    ( (*m_mu_nCSCEtaHits)[counter] == 0) &&
    ( (*m_mu_nMDTBEEHits)[counter] == 0) &&
    ( (*m_mu_nMDTEEHits)[counter] == 0) &&
    ( (*m_mu_nMDTBIS78Hits)[counter] == 0) &&
    (fabs( (*m_mu_eta)[counter]) < 1.05) &&
    !( mu_phiSector == 9 && fabs( (*m_mu_eta)[counter]) > 0.2 && fabs( (*m_mu_eta)[counter]) < 0.35) &&
    !(( mu_phiSector == 4 || mu_phiSector == 6) && fabs( (*m_mu_eta)[counter]) > 0.85) &&
    !( mu_phiSector == 13 && (*m_mu_eta)[counter] > 0 && (*m_mu_eta)[counter] < 0.2);
    
    mu_is2St = mu_is2StBarrel;
    return mu_is2St;
    
}

//-----MuonMomentum-Significance-(calculated before smearing)-----//
float DerivationFramework::MCPHighPtRecommendations::getSigDiffQOverP(unsigned int counter) const {
    
    float mu_sig_diff_qoverp(0);
    float mu_diff_qoverp     = 0;
    float mu_err_diff_qoverp = 1;
    
    mu_diff_qoverp     = 1.e6 * ((*m_mu_me_qoverp_exPV)[counter] - (*m_mu_id_qoverp_exPV)[counter]);
    mu_err_diff_qoverp = 1.e6 * sqrt( (*m_mu_me_cov_qoverp_exPV)[counter] + (*m_mu_id_cov_qoverp_exPV)[counter]);
    // Signed momentum difference significance
    mu_sig_diff_qoverp = mu_diff_qoverp / mu_err_diff_qoverp;
    return mu_sig_diff_qoverp;
    
}

// Identify the phi sector for a given muon phi
int DerivationFramework::MCPHighPtRecommendations::getPhiSector(float muonPhiSigned) const
{
    // Map [-Pi..Pi] to [0..2Pi]
    float muonPhi = muonPhiSigned;
    float PI(3.14159);
    if (muonPhi < 0) muonPhi += 2*PI;
    
    int sector = -1;
    
    float widthLarge = 0.50;  // (BML)
    float widthSmall = 0.36;  // (BOS)
    
    float overlap = (8*(widthLarge + widthSmall) - 2*PI ) / 16.;
    
    widthLarge -= overlap;
    widthSmall -= overlap;
    
    float edge[18];
    edge[0]  = 0;
    edge[17] = 2*PI;
    
    for (int i=0; i <= 7; i++){
        edge[2*i+1] = i*(widthLarge + widthSmall) + widthLarge/2.;
        edge[2*i+2] = i*(widthLarge + widthSmall) + widthLarge/2 + widthSmall;
    }
    
    for (int j=0; j <= 16; j++){
        //cout << edge[j] << " - " << edge[j+1] << endl;
        if (muonPhi >= edge[j] && muonPhi < edge[j+1]){
            sector = j+1;
            break;
        }
    }
    
    if (sector == 17) sector = 1;
    
    if (sector == -1) ATH_MSG_ERROR("Error in getPhiSector: sector not found");
    
    return sector;
    
}

// Collect data from StoreGate
StatusCode DerivationFramework::MCPHighPtRecommendations::retrieveFromStoreGate(void)  const {
    
    // Build branch names
    // Gotcha: in NTUP_COMMON branch names go as:
    // mu_muid_blah for muid
    // mu_staco_blah for staco
    // mu_blah for 3rd chain
    std::string mu_isCombinedMuonName = "mu_"+m_collName+"isCombinedMuon";
    std::string mu_ptName = "mu_"+m_collName+"pt";
    std::string mu_etaName = "mu_"+m_collName+"eta";
    std::string mu_qoverp_exPVName = "mu_"+m_collName+"qoverp_exPV";
    std::string mu_phi_exPVName = "mu_"+m_collName+"phi_exPV";
    std::string mu_id_d0_exPVName = "mu_"+m_collName+"id_d0_exPV";
    std::string mu_id_z0_exPVName = "mu_"+m_collName+"id_z0_exPV";
    std::string mu_id_qoverp_exPVName = "mu_"+m_collName+"id_qoverp_exPV";
    std::string mu_me_qoverp_exPVName = "mu_"+m_collName+"me_qoverp_exPV";
    std::string trk_ptName = "trk_pt";
    std::string trk_etaName = "trk_eta";
    std::string trk_phi_wrtPVName = "trk_phi_wrtPV";
    std::string mu_phiName = "mu_"+m_collName+"phi";
    std::string mu_nMDTBIHitsName = "mu_"+m_collName+"nMDTBIHits";
    std::string mu_nMDTBMHitsName = "mu_"+m_collName+"nMDTBMHits";
    std::string mu_nMDTBOHitsName = "mu_"+m_collName+"nMDTBOHits";
    std::string mu_nMDTEIHitsName = "mu_"+m_collName+"nMDTEIHits";
    std::string mu_nCSCEtaHitsName = "mu_"+m_collName+"nCSCEtaHits";
    std::string mu_nMDTEMHitsName = "mu_"+m_collName+"nMDTEMHits";
    std::string mu_nMDTEOHitsName = "mu_"+m_collName+"nMDTEOHits";
    std::string mu_nMDTEEHitsName = "mu_"+m_collName+"nMDTEEHits";
    std::string mu_nRPCLayer1PhiHitsName = "mu_"+m_collName+"nRPCLayer1PhiHits";
    std::string mu_nRPCLayer2PhiHitsName = "mu_"+m_collName+"nRPCLayer2PhiHits";
    std::string mu_nRPCLayer3PhiHitsName = "mu_"+m_collName+"nRPCLayer3PhiHits";
    std::string mu_nMDTBIS78HitsName = "mu_"+m_collName+"nMDTBIS78Hits";
    std::string mu_nTGCLayer1PhiHitsName = "mu_"+m_collName+"nTGCLayer1PhiHits";
    std::string mu_nTGCLayer2PhiHitsName = "mu_"+m_collName+"nTGCLayer2PhiHits";
    std::string mu_nTGCLayer3PhiHitsName = "mu_"+m_collName+"nTGCLayer3PhiHits";
    std::string mu_nTGCLayer4PhiHitsName = "mu_"+m_collName+"nTGCLayer4PhiHits";
    std::string mu_nCSCPhiHitsName = "mu_"+m_collName+"nCSCPhiHits";
    std::string mu_nCSCUnspoiledEtaHitsName = "mu_"+m_collName+"nCSCUnspoiledEtaHits";
    std::string mu_nMDTBEEHitsName = "mu_"+m_collName+"nMDTBEEHits";
    std::string mu_me_cov_qoverp_exPVName = "mu_"+m_collName+"me_cov_qoverp_exPV";
    std::string mu_id_cov_qoverp_exPVName = "mu_"+m_collName+"id_cov_qoverp_exPV";
    std::string mu_ptcone30Name = "mu_"+m_collName+"ptcone30";
    
    // Retrieves
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_isCombinedMuon,mu_isCombinedMuonName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_pt,mu_ptName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_eta,mu_etaName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_qoverp_exPV,mu_qoverp_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_phi_exPV,mu_phi_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_id_d0_exPV,mu_id_d0_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_id_z0_exPV,mu_id_z0_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_id_qoverp_exPV,mu_id_qoverp_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_me_qoverp_exPV,mu_me_qoverp_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_trk_pt,trk_ptName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_trk_eta,trk_etaName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_trk_phi_wrtPV,trk_phi_wrtPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_phi,mu_phiName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTBIHits,mu_nMDTBIHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTBMHits,mu_nMDTBMHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTBOHits,mu_nMDTBOHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTEIHits,mu_nMDTEIHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nCSCEtaHits,mu_nCSCEtaHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTEMHits,mu_nMDTEMHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTEOHits,mu_nMDTEOHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTEEHits,mu_nMDTEEHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nRPCLayer1PhiHits,mu_nRPCLayer1PhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nRPCLayer2PhiHits,mu_nRPCLayer2PhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nRPCLayer3PhiHits,mu_nRPCLayer3PhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTBIS78Hits,mu_nMDTBIS78HitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nTGCLayer1PhiHits,mu_nTGCLayer1PhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nTGCLayer2PhiHits,mu_nTGCLayer2PhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nTGCLayer3PhiHits,mu_nTGCLayer3PhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nTGCLayer4PhiHits,mu_nTGCLayer4PhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nCSCPhiHits,mu_nCSCPhiHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nCSCUnspoiledEtaHits,mu_nCSCUnspoiledEtaHitsName));
    CHECK(evtStore()->retrieve((const std::vector<int>*&)m_mu_nMDTBEEHits,mu_nMDTBEEHitsName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_me_cov_qoverp_exPV,mu_me_cov_qoverp_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_id_cov_qoverp_exPV,mu_id_cov_qoverp_exPVName));
    CHECK(evtStore()->retrieve((const std::vector<float>*&)m_mu_ptcone30,mu_ptcone30Name));   
 
    return StatusCode::SUCCESS;
    
}
