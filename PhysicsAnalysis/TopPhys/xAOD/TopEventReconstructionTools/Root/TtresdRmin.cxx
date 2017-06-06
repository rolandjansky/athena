/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// TTBarLeptonJetsBuilder_dRmin tool                                             //
//                                                                               //
// This tool provides functionality to calculated the ttbar mass using the       //
// dRmin method                                                                  //
//                                                                               //
// (1) If the event contains a high mass jet above a given threshold, the        //
//     mass is calculated using the high mass jet, the closest jet to this       //
//     one, and the jet closest to the leptonic W-boson                          //
//                                                                               //
// (2) If the event does not contain a high mass jet and at least four jets,     //
//     the dRmin method removes jets passing a specific cut requirement          //
//     correlated to its mass. This procedure is repeated as long as there       //
//     are at least 4 jets available.                                            //
//                                                                               //
// Note: This procedure also calculates the mass for cases with only 2 jets.     //
// These events have to be filtered in the cutflow!                              //
//                                                                               //
// Contact: lmasetti@cern.ch                                                     //
//          theck@cern.ch                                                        //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////


#include "TopEventReconstructionTools/TtresdRmin.h"

//==========================================================================================================
// Class constructor
//
TtresdRmin::TtresdRmin(TString units, bool verbose) {

  m_init = false;
  m_verbose = verbose;

  if(units == "MeV") {
    m_Units = 1000.0;
  } else if(units == "GeV") {
    m_Units = 1.0;
  } else {
    cout  << " ERROR in in TtresdRmin :: Unit setting '" << units << "' doest not match pattern of GeV or MeV" << endl;
  }
}


//==========================================================================================================
// Initialize parameters for tool
//
void TtresdRmin::Init(double highJetMass, double minJetPt, double dRCutOffset, double dRCutMassFactor) {
  //Set parameters
  m_highM           = highJetMass;
  m_minJetPt        = minJetPt;
  m_dRCutOffset     = dRCutOffset;
  m_dRCutMassFactor = dRCutMassFactor;

  //Reset
  m_jet_indices.clear();
  m_highMassJetIndex = -1;
  m_highestJetMass   = 0.0;

  //Initialization status
  m_init = true;
}


//==========================================================================================================
// Setup event with kinematic objects
//
bool TtresdRmin::SetupEvent(vector<TLorentzVector*> &jets, TLorentzVector &lepton, TLorentzVector &neutrino) {

  if(!m_init) {
    cout << " ERROR in TtresdRmin :: Tool not initialized! Initialize tool before usage!!!" << endl;
    return false;
  }

  //Set objects
  m_jets      = jets;
  m_lepton    = lepton;
  m_neutrino  = neutrino;
  m_Wleptonic = lepton + neutrino;

  //Reset
  m_jet_indices.clear();
  m_highMassJetIndex = -1;
  m_highestJetMass = 0.0;


  //===== Search for high mass jet =====
  int    highMJetIdx = -1;                  //Index of jet with highest mass above threshold
  double highM       = m_highM*m_Units;     //Threshold for high mass jet
  double minJetPt    = m_minJetPt*m_Units;  //Threshold for jet pT

  //Loop jets
  for(int ijet = 0; ijet < (int)jets.size(); ijet++) {

    //pT requirmenet
    if(jets.at(ijet)->Pt() <= minJetPt) continue; //next jet

    //high mass requirement
    if(jets.at(ijet)->M() <= highM) continue; //next jet

    highMJetIdx = ijet;
    highM = jets.at(ijet)->M();
  }//end of jet loop

  if(highMJetIdx >= 0) {
    m_highMassJetIndex = highMJetIdx;
    m_highestJetMass = highM;
    FindJetIndices_HM();
  } else {
    FindJetIndices_LM();
  }

  return true;
}


//==========================================================================================================
// Get invariant ttbar mass
//
double TtresdRmin::GetMttdRmin() {

  double mtt_dRmin = 0.0;

  if(m_jet_indices.size() > 0) {
    TLorentzVector dRsumJet(0,0,0,0);
    for(unsigned int ijet = 0; ijet < m_jet_indices.size(); ijet++) {
      dRsumJet += *(m_jets.at(m_jet_indices.at(ijet)));
    }
    mtt_dRmin = (dRsumJet + m_Wleptonic).M();
  }

  return mtt_dRmin;
}

//==========================================================================================================
// Get ttbar system TLorentzVector
//
TLorentzVector TtresdRmin::GetTlvdRmin() {

  TLorentzVector tt_dRmin(0,0,0,0);

  if(m_jet_indices.size() > 0) {
    TLorentzVector dRsumJet(0,0,0,0);
    for(unsigned int ijet = 0; ijet < (unsigned int)m_jet_indices.size(); ijet++) {
      dRsumJet += *(m_jets.at(m_jet_indices.at(ijet)));
    }
    tt_dRmin = dRsumJet + m_Wleptonic;
  }

  return tt_dRmin;
}


//==========================================================================================================
// Find jet indices for reconstruction for events with a high mass jet
//
bool TtresdRmin::FindJetIndices_HM() {

  //===== Find closest jet to high mass jet =====
  double dR_had = 999.9;
  int closest_jetIdx_had = -1;

  //Loop m_jets
  for(int ijet = 0; ijet < (int)m_jets.size(); ijet++) {
    if(ijet == m_highMassJetIndex) continue; //next jet
    double dR = m_jets.at(m_highMassJetIndex)->DeltaR(*(m_jets.at(ijet)));
    if(dR < dR_had) {
      closest_jetIdx_had = ijet;
      dR_had = dR;
    }
  }//end of jet loop


  //===== Find closest jet to leptonic W-boson =====
  double dR_lep = 999.9;
  int closest_jetIdx_lep = -1;

  //Loop jets
  for(int ijet = 0; ijet < (int)m_jets.size(); ijet++) {
    if(ijet == m_highMassJetIndex) continue; //next jet
    if(ijet == closest_jetIdx_had) continue; //next jet
    double dR = m_Wleptonic.DeltaR(*(m_jets.at(ijet)));
    if(dR < dR_lep) {
      closest_jetIdx_lep = ijet;
      dR_lep = dR;
    }
  }//end of jet loop

  //===== Save jet indices used for reconstruction =====
  m_jet_indices.push_back(m_highMassJetIndex);

  if(closest_jetIdx_had >= 0){
    m_jet_indices.push_back(closest_jetIdx_had);
  } else {
    if(m_verbose) cout << " WARNING in TtresdRmin :: Can not find jet close to high mass jet" << endl;
  }

  if(closest_jetIdx_lep >= 0) {
    m_jet_indices.push_back(closest_jetIdx_lep);
  } else {
    if(m_verbose) cout << " WARNING in TtresdRmin :: Can not find jet close to leptonic W-boson" << endl;
  }

  return true;
}


//==========================================================================================================
// Find jet indices for reconstruction for events without a high mass jet
//
bool TtresdRmin::FindJetIndices_LM() {

  //jetflag vector to flag jets rejected by dRmin method
  vector<bool> jetflags(m_jets.size(), true);

  //do dRmin procedure if at least 4 jets are available
  bool repeat_dRmin = ((int)m_jets.size() > 3);

  //Loop dRmin method
  while(repeat_dRmin) {
    repeat_dRmin = false;

    double dR_max = 0.0;  //Maximum dR passing dRmin cut requirement
    int index_max = -1;   //Index of jet with dR_max

    //Number of goodflagged processed jets (outer loop)
    int nijet = 0;

    //Outer jet loop
    for(int ijet = 0; ijet < (int)m_jets.size(); ijet++) {

      if(!jetflags.at(ijet)) continue; //next outer jet

      //Use maximum of 4 goodflagged jets
      if(nijet > 3) break;
      nijet++;

      double dR_min = 999.9;

      double dR_lep = m_jets.at(ijet)->DeltaR(m_lepton);
      dR_min = dR_lep;

      //Number of goodflagged processed jets (inner loop)
      int njjet = 0;

      //Inner jet loop
      for(int jjet = 0; jjet < (int)m_jets.size(); jjet++) {

	if(!jetflags.at(jjet)) continue; //next inner jet

	//Use maximum of 4 goodflagged jets
	if(njjet > 3) break;
	njjet++;

	if(jjet == ijet) continue; //next inner jet

	double dR_jet = m_jets.at(ijet)->DeltaR(*(m_jets.at(jjet)));
	if(dR_jet < dR_min){
	  dR_min = dR_jet;
	}
      } //end of inner jet loop

      if(dR_min > dR_max) {
	//dRmin specific cut (defined for GeV, needs m_Units correction on mass)
	double dR_cut = m_dRCutOffset - m_dRCutMassFactor * m_jets.at(ijet)->M()/m_Units;
	if(dR_min > dR_cut) {
	  dR_max = dR_min;
	  index_max = ijet;
	}
      }
    }//end of outer jet loop

    if(index_max >= 0) {
      jetflags.at(index_max) = false;
      if(count(jetflags.begin(), jetflags.end(), true) > 3) repeat_dRmin = true;
    }
  }//end of while(repeat)

  int nijet = 0;
  //Save selected jet indices
  for(int ijet = 0; ijet < (int)m_jets.size(); ijet++) {
    if(!jetflags.at(ijet)) continue; //next jet

    //Use maximum of 4 goodflagged jets
    if(nijet > 3) break;
    nijet++;

    m_jet_indices.push_back(ijet);
  }

  return true;
}

