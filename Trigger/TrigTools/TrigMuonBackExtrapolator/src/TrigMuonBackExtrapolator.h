/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONBACKEXTRAPOLATOR_H
#define TRIGMUONBACKEXTRAPOLATOR_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigMuonToolInterfaces/ITrigMuonBackExtrapolator.h"



class TrigMuonBackExtrapolator: public AthAlgTool, 
                                virtual public ITrigMuonBackExtrapolator
{
    public:
    TrigMuonBackExtrapolator(const std::string&, 
                             const std::string&,
			     const IInterface* );
    
    ~TrigMuonBackExtrapolator();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
    
    
    StatusCode give_eta_phi_at_vertex (const MuonFeature*, // input muon track
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window

    StatusCode give_eta_phi_at_vertex (double pt,          // pt of muon track
                                       const MuonFeature*, // input muon track
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window


    StatusCode give_eta_phi_at_vertex( const MuonFeature*, // input muon track
                                       double ZetaID,      // Z vertex from ID
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window

    StatusCode give_eta_phi_at_vertex( double pt,          // pt of muon track
                                       const MuonFeature*, // input muon track
                                       double ZetaID,      // Z vertex from ID
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window


    StatusCode loose_window_match (const MuonFeature*,     // input muon track  
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window
    
    StatusCode loose_window_match (double pt,              // pt of muon track
                                   const MuonFeature*,     // input muon track  
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window		     

    StatusCode tight_window_match (const MuonFeature*,     // input muon track
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window   
   
    StatusCode tight_window_match (double pt,              // pt of muon track
                                   const MuonFeature*,     // input muon track
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window
  
  
    double give_loose_chi2 (const MuonFeature*,     // input muon track 
                            const TrigInDetTrack*); // input ID track
  
  
    double give_tight_chi2  (const MuonFeature*,     // input muon track 
                             const TrigInDetTrack*); // input ID track

    // Use xAOD EDM
    StatusCode give_eta_phi_at_vertex (const xAOD::L2StandAloneMuon*, // input muon track
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window

    StatusCode give_eta_phi_at_vertex (double pt,          // pt of muon track
                                       const xAOD::L2StandAloneMuon*, // input muon track
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window


    StatusCode give_eta_phi_at_vertex( const xAOD::L2StandAloneMuon*, // input muon track
                                       double ZetaID,      // Z vertex from ID
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window

    StatusCode give_eta_phi_at_vertex( double pt,          // pt of muon track
                                       const xAOD::L2StandAloneMuon*, // input muon track
                                       double ZetaID,      // Z vertex from ID
				       double& extEta,     // vertex eta
				       double& sigmaEta,   // sigma vertex eta
				       double& extPhi,     // vertex phi
				       double& sigmaPhi,   // sigma vertex phi
				       double PT);         // PT of the window


    StatusCode loose_window_match (const xAOD::L2StandAloneMuon*,     // input muon track  
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window
    
    StatusCode loose_window_match (double pt,              // pt of muon track
                                   const xAOD::L2StandAloneMuon*,     // input muon track  
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window		     

    StatusCode tight_window_match (const xAOD::L2StandAloneMuon*,     // input muon track
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window   
   
    StatusCode tight_window_match (double pt,              // pt of muon track
                                   const xAOD::L2StandAloneMuon*,     // input muon track
		                   const  TrigInDetTrack*, // input ID track
                                   double winPT,           // PT of the window
                                   double weight);         // weight of window
  
  
    void init_LUT();


    private:
    BooleanProperty m_aligned;
    BooleanProperty m_dataset;

    double m_data_Barrel_Param[2][2][2]; 
    double m_data_Barrel_Sigmas[2][2][2][2]; 
    double m_data_Endcap_TriggerST_Param[4][12][2][2][2][2];
    double m_data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2];
    double m_data_Endcap_InnerST_Param[4][12][2][2][2];
    double m_data_Endcap_InnerST_PhiSigmas[4][12][2][2][2];
    double m_data_Endcap_InnerST_EtaSigmas[2][2][2][2][2]; 

};

#endif  // TRIGMUONBACKEXTRAPOLATOR_H
