/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGMUONBACKEXTRAPOLATOR_H__ 
#define __ITRIGMUONBACKEXTRAPOLATOR_H__ 

#include "GaudiKernel/IAlgTool.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

static const InterfaceID IID_ITrigMuonBackExtrapolator("ITrigMuonBackExtrapolator", 1 , 0); 

class ITrigMuonBackExtrapolator: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigMuonBackExtrapolator;
  }


  // extrapolate without using the ID vertex measurement
  virtual 
  StatusCode give_eta_phi_at_vertex (const MuonFeature*, // input muon track
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window

  virtual
  StatusCode give_eta_phi_at_vertex (double pt,          // pt of muon track
                                     const MuonFeature*, // input muon track
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window

  // extrapolate using the ID vertext measurement
  virtual 
  StatusCode give_eta_phi_at_vertex( const MuonFeature*, // input muon track
                                     double ZetaID,      // Z vertex from ID
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window

  virtual 
  StatusCode give_eta_phi_at_vertex( double pt,          // pt of muon track
                                     const MuonFeature*, // input muon track
                                     double ZetaID,      // Z vertex from ID
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window


  // match ID track and Muon track applying a loose cut window
  virtual 
  StatusCode loose_window_match (const MuonFeature*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window
	   
  virtual 
  StatusCode loose_window_match (double pt,              // pt of muon track
                                 const MuonFeature*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window


  // match ID track and Muon track applying a tight cut window
  virtual 
  StatusCode tight_window_match (const MuonFeature*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window
  
  virtual 
  StatusCode tight_window_match (double pt,              // pt of muon track
                                 const MuonFeature*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window
 
 
  // return a Chi^2 between ID track and Muon track position with loose parameters
  virtual 
  double give_loose_chi2 (const MuonFeature*,          // input muon track 
                          const TrigInDetTrack*) = 0;  // input ID track
  
  
  // return a Chi^2 between ID track and Muon track position with tight parameters
  virtual 
  double give_tight_chi2  (const MuonFeature*,          // input muon track 
                           const TrigInDetTrack*) = 0;  // input ID track
  
  // Use xAOD EDM
  // extrapolate without using the ID vertex measurement
  virtual 
  StatusCode give_eta_phi_at_vertex (const xAOD::L2StandAloneMuon*, // input muon track
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window

  virtual
  StatusCode give_eta_phi_at_vertex (double pt,          // pt of muon track
                                     const xAOD::L2StandAloneMuon*, // input muon track
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window

  // extrapolate using the ID vertext measurement
  virtual 
  StatusCode give_eta_phi_at_vertex( const xAOD::L2StandAloneMuon*, // input muon track
                                     double ZetaID,      // Z vertex from ID
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window

  virtual 
  StatusCode give_eta_phi_at_vertex( double pt,          // pt of muon track
                                     const xAOD::L2StandAloneMuon*, // input muon track
                                     double ZetaID,      // Z vertex from ID
				     double& extEta,     // vertex eta
				     double& sigmaEta,   // sigma vertex eta
				     double& extPhi,     // vertex phi
				     double& sigmaPhi,   // sigma vertex phi
				     double PT) = 0;     // PT of the window


  // match ID track and Muon track applying a loose cut window
  virtual 
  StatusCode loose_window_match (const xAOD::L2StandAloneMuon*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window
	   
  virtual 
  StatusCode loose_window_match (double pt,              // pt of muon track
                                 const xAOD::L2StandAloneMuon*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window


  // match ID track and Muon track applying a tight cut window
  virtual 
  StatusCode tight_window_match (const xAOD::L2StandAloneMuon*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window
  
  virtual 
  StatusCode tight_window_match (double pt,              // pt of muon track
                                 const xAOD::L2StandAloneMuon*,     // input muon track	       
		                 const  TrigInDetTrack*, // input ID track
                                 double winPT,           // PT of the window
                                 double weight) = 0;     // weight of window
 
 
};

#endif


