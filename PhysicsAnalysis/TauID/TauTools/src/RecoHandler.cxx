/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////// 
/// Name    : RecoHandler.cxx
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Authors: Jyothsna Komaragiri, Dugan O'Neil
/// Date: April 2008
///
/// DESCRIPTION:
///
/// This class is a simplified example of accessing reconstructed information
/// for taus reconstructed in release 14.x.x
///
//////////////////////////////////////////////////////////////////////////////////

#include "TauTools/RecoHandler.h"

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

RecoHandler::RecoHandler() {}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

RecoHandler::~RecoHandler() {}

/////////////////////////////////////////////////////////////////////////////////////
/** Check the tau type: tauRec, tau1p3p or overlap candidate */
int RecoHandler::TauType(const Analysis::TauJet *tauJet){
  // this function returns the type of tau candidate
  // 0 none
  // 1 tauRec
  // 2 tau1p3p
  // 3 both
  // 4 all

  if ((tauJet->hasAuthor( TauJetParameters::tauRec)) && (tauJet->hasAuthor( TauJetParameters::tau1P3P)))
    return TAU_BOTH;
  
  if (tauJet->hasAuthor( TauJetParameters::tau1P3P))
    return TAU_1P3P;

  if (tauJet->hasAuthor( TauJetParameters::tauRec))
    return TAU_REC;
  
  return TAU_NONE;
}

/////////////////////////////////////////////////////////////////////////////////////
// Methods related to reconstructed tau Jets
//
/** TauRecJet selection */
bool RecoHandler::selectTauRecJet( const Analysis::TauJet *tauJet, int algo, float cut, bool useGoodCharge ) {

  bool isSelected = false;

  bool goodCharge = false;
    
  if (useGoodCharge)
    goodCharge = fabs( tauJet->charge() ) == 1;
  else
    goodCharge = true;

  //for discriminant information       
  const Analysis::TauPID *p_tauid = tauJet->tauID();
  bool disc_cut = false;

  switch (algo) {
  case 1:
    disc_cut = p_tauid->discriminant(TauJetParameters::BDTJetScore) > cut;
    break;
  case 2:
    disc_cut = p_tauid->discriminant(TauJetParameters::BDTEleScore) > cut;
    break;
    //  case 3:
    //    disc_cut = p_tauid->discriminant(TauJetParameters::TauJetNeuralNetwork) > cut;
    //    break;
    //  case 4:
    //    disc_cut = p_tauid->discriminant(TauJetParameters::TauENeuralNetwork) > cut;
    //    break;
  case 5:
    disc_cut = p_tauid->discriminant(TauJetParameters::Likelihood) > cut;
    break;
  default:
    disc_cut = false;
  }
  
  isSelected = disc_cut && goodCharge;

  return isSelected;
}

//////////////////////////////
/** TauRecJet selection: Return the container of good taus*/
Analysis::TauJetContainer* RecoHandler::selectTauRecJet( const Analysis::TauJetContainer *tau, int algo, float cut ) {
  
  /** define iterators for the input container */
  Analysis::TauJetContainer::const_iterator tauItr  = tau->begin();
  Analysis::TauJetContainer::const_iterator tauItrE = tau->end();
  
  //Output container
  Analysis::TauJetContainer* SelectedContainer = new Analysis::TauJetContainer(SG::VIEW_ELEMENTS);
  
  //Loop over input container, put selected candidates in output container
  for (; tauItr != tauItrE; ++tauItr) {//reco loop
    
    bool isSelected = false;
    
    bool goodCharge = fabs( (*tauItr)->charge() ) == 1;
    
    //for discriminant information       
    const Analysis::TauPID *p_tauid = (*tauItr)->tauID();
    bool disc_cut = false;

    switch (algo) {
    case 1:
      disc_cut = p_tauid->discriminant(TauJetParameters::BDTJetScore) > cut;
      break;
    case 2:
      disc_cut = p_tauid->discriminant(TauJetParameters::BDTEleScore) > cut;
      break;
      //    case 3:
      //disc_cut = p_tauid->discriminant(TauJetParameters::TauJetNeuralNetwork) > cut;
      //break;
      //    case 4:
      //      disc_cut = p_tauid->discriminant(TauJetParameters::TauENeuralNetwork) > cut;
      //      break;
    case 5:
      disc_cut = p_tauid->discriminant(TauJetParameters::Likelihood) > cut;
      break;
    default:
      disc_cut = false;
    }

    isSelected = disc_cut && goodCharge;
    
    if(isSelected) {
      const Analysis::TauJet* myTau = *tauItr;      
      SelectedContainer->push_back( const_cast<Analysis::TauJet*> (myTau) );
    }
  }//reco loop
  
  return SelectedContainer;
}

/////////////////////////
/** Tau1P3PJet selection */
// Removed for tauEvent-00-06-00
/*
bool RecoHandler::selectTau1P3PJet( const Analysis::TauJet *tauJet, int algo, float cut, 
				    bool useGoodCharge, bool useEVeto, bool useMuVeto) {
  bool isSelected = false;
  
  bool goodCharge = false;
  
  if (useGoodCharge)
    goodCharge = fabs( tauJet->charge() ) == 1;
  else
    goodCharge = true;

  //for discriminant information       
  const Analysis::TauPID *p_tauid = tauJet->tauID();

  // Veto Electron and Muon....available only for 13.0.xx
  bool eveto  = false;
  bool muveto = false;
  
  if (useEVeto == true)
    eveto = !(p_tauid->flag( TauJetParameters::ElectronFlag));
  else
    eveto = true;

  if (useMuVeto == true)
    muveto = !(p_tauid->flag( TauJetParameters::MuonFlag));
  else
    muveto = true;

  bool disc_cut = false;

  switch (algo) {
  case 1:
    disc_cut = p_tauid->discriminant(TauJetParameters::DiscCut) > cut;
    break;
  case 2:
    disc_cut = p_tauid->discriminant(TauJetParameters::DiscNN) > cut;
    break;
  case 3:
    disc_cut = p_tauid->discriminant(TauJetParameters::EfficNN) > cut;
    break;
  case 4:
    disc_cut = p_tauid->discriminant(TauJetParameters::DiscPDERS) > cut;
    break;
  case 5:
    disc_cut = p_tauid->discriminant(TauJetParameters::EfficPDERS) > cut;
    break;
  default:
    disc_cut = false;
  }

  isSelected = disc_cut && goodCharge && eveto && muveto;

  return isSelected;
}
*/
//////////////////////////////
/** TauRecJet selection: Return the container of good taus*/
// Removed for tauEvent-00-06-00
/*
Analysis::TauJetContainer* RecoHandler::selectTau1P3PJet( const Analysis::TauJetContainer *tau, int algo, float cut ) {
  
  // define iterators for the input container 
  Analysis::TauJetContainer::const_iterator tauItr  = tau->begin();
  Analysis::TauJetContainer::const_iterator tauItrE = tau->end();
  
  //Output container
  Analysis::TauJetContainer* SelectedContainer = new Analysis::TauJetContainer(SG::VIEW_ELEMENTS);
  
  //Loop over input container, put selected candidates in output container
  for (; tauItr != tauItrE; ++tauItr) {//reco loop
    
    bool isSelected = false;
    
    bool goodCharge = fabs( (*tauItr)->charge() ) == 1;
    
    //for discriminant information       
    const Analysis::TauPID *p_tauid = (*tauItr)->tauID();
    
    // Veto Electron and Muon....available only for 13.0.xx
    bool emuveto = !( p_tauid->flag( TauJetParameters::ElectronFlag) || p_tauid->flag( TauJetParameters::MuonFlag) );
    
    bool disc_cut = false;
    switch (algo) {
    case 1:
      disc_cut = p_tauid->discriminant(TauJetParameters::DiscCut) > cut;
      break;
    case 2:
      disc_cut = p_tauid->discriminant(TauJetParameters::DiscNN) > cut;
      break;
    case 3:
      disc_cut = p_tauid->discriminant(TauJetParameters::EfficNN) > cut;
      break;
    case 4:
      disc_cut = p_tauid->discriminant(TauJetParameters::DiscPDERS) > cut;
      break;
    case 5:
      disc_cut = p_tauid->discriminant(TauJetParameters::EfficPDERS) > cut;
      break;
    default:
      disc_cut = false;
      
    }
    
    isSelected = disc_cut && goodCharge && emuveto;
    
    if(isSelected) {
      Analysis::TauJet* myTau = *tauItr;      
      SelectedContainer->push_back( myTau );
    }
  }//reco loop
  
  return SelectedContainer;
}

/////////////////////////
/////////////////////////
*/
