/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTrigChainList.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           List of trigger chains, and associated analysis functions
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauTrigChainList.h"

#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"

#include <math.h>
#include <iostream>
#include <iomanip>

using namespace std;

ClassImp(TrigTauTrigChainList)  

// const bool _debug=true;
const bool _debug=false;

TrigTauTrigChainList::TrigTauTrigChainList() : 
  _maxL1dR(0.15), 
  _maxL2dR(0.1), 
  _maxEFdR(0.1), // used for both MC->EF match and for L2->EF geometrical match
  _maxRedR(0.2), 
  _useRoiWord(true),
  _hltAuthorType(TrigTauFlags::ANYAUTHOR) {}
  //_hltAuthorType(TrigTauFlags::IDSCAN) {}

TrigTauTrigChainList::~TrigTauTrigChainList() {}

void TrigTauTrigChainList::printChainList() const {
  if (_chainList.size() == 1) {
    cout << " There  is " << _chainList.size() << " chain" << endl;
  } else {
    cout << " There are " << _chainList.size() << " chains" << endl;
  }

  for (std::vector<TrigTauTrigChain>::const_iterator
	 itr = _chainList.begin();
       itr != _chainList.end(); ++itr) {
    itr->print();
  }
}

unsigned int 
TrigTauTrigChainList::nTrigChains() const {
  return _chainList.size();
}

const TrigTauTrigChain&
TrigTauTrigChainList::trigChainEntry(unsigned int i) const {
  return _chainList[i];
}

unsigned int 
TrigTauTrigChainList::nPassL1Chains() const {
  return _tauL1.size();
}

const TrigTauTrigChain&
TrigTauTrigChainList::passL1ChainEntry(unsigned int i) const {
  return *_tauL1[i];
}

unsigned int 
TrigTauTrigChainList::nPassL2Chains() const {
  return _tauL2.size();
}
unsigned int 
TrigTauTrigChainList::nPassL2TrkChains() const {
  return _tauL2Trk.size();
}
unsigned int 
TrigTauTrigChainList::nPassL2CalChains() const {
  return _tauL2Cal.size();
}

const TrigTauTrigChain&
TrigTauTrigChainList::passL2ChainEntry(unsigned int i) const {
  return *_tauL2[i];
}
const TrigTauTrigChain&
TrigTauTrigChainList::passL2TrkChainEntry(unsigned int i) const {
  return *_tauL2Trk[i];
}
const TrigTauTrigChain&
TrigTauTrigChainList::passL2CalChainEntry(unsigned int i) const {
  return *_tauL2Cal[i];
}

unsigned int 
TrigTauTrigChainList::nPassEFChains() const {
  return _tauEF.size();
}

const TrigTauTrigChain&
TrigTauTrigChainList::passEFChainEntry(unsigned int i) const {
  return *_tauEF[i];
}

const TrigTauTrigChain*
TrigTauTrigChainList::getChain(const TLorentzVector& goodTau) const {
  float dRMin = 10000;
  const TrigTauTrigChain* chain(0);

  for (std::vector<TrigTauTrigChain>::const_iterator 
	 itr = _chainList.begin(); itr != _chainList.end(); ++itr ) {

    // Could also use the eta/phi of the highest defined trigger
    // object in the chain...
    float dR = goodTau.DeltaR(*itr->L1);
    if (dR < dRMin) { dRMin = dR; chain = &(*itr); }
  }

  if (dRMin < _maxL1dR) return chain;
  return 0;
}

const TrigTauTrigChain*
TrigTauTrigChainList::getL1Chain(const TLorentzVector& goodTau) const {
  float dRMin = 10000;
  const TrigTauTrigChain* chain(0);

  for (std::vector<const TrigTauTrigChain*>::const_iterator 
	 itr = _tauL1.begin(); itr != _tauL1.end(); ++itr ) {

    // Could also use the eta/phi of the highest defined trigger
    // object in the chain...
    float dR = goodTau.DeltaR(*(*itr)->L1);
    if (dR < dRMin) { dRMin = dR; chain = *itr; }
  }

  if (dRMin < _maxL1dR) return chain;
  return 0;
}

const TrigTauTrigChain*
TrigTauTrigChainList::getL2CalChain(const TLorentzVector& goodTau) const {
  // Match L2 eta/phi with visible truth

  float dRMin = 10000;
  const TrigTauTrigChain* chain(0);

  for (std::vector<const TrigTauTrigChain*>::const_iterator 
	 itr = _tauL2Cal.begin(); itr != _tauL2Cal.end(); ++itr ) {

    // Should really specify the calo-based 4-vector here
    float dR = goodTau.DeltaR(*(*itr)->L2);  
    if (dR < dRMin) { dRMin = dR; chain = *itr; }
  }

  if (dRMin < _maxL2dR) return chain;
  return 0;
}

const TrigTauTrigChain*
TrigTauTrigChainList::getL2TrkChain(const TLorentzVector& goodTau) const {
  // Match L2 eta/phi with visible truth

  float dRMin = 10000;
  const TrigTauTrigChain* chain(0);

  for (std::vector<const TrigTauTrigChain*>::const_iterator 
	 itr = _tauL2Trk.begin(); itr != _tauL2Trk.end(); ++itr ) {

    // Should really specify the track-based 4-vector here
    float dR = goodTau.DeltaR(*(*itr)->L2);
    if (dR < dRMin) { dRMin = dR; chain = *itr; }
  }

  if (dRMin < _maxL2dR) return chain;
  return 0;
}

const TrigTauTrigChain*
TrigTauTrigChainList::getL2Chain(const TLorentzVector& goodTau) const {
  // Match L2 eta/phi with visible truth

  float dRMin = 10000;
  const TrigTauTrigChain* chain(0);

  for (std::vector<const TrigTauTrigChain*>::const_iterator 
	 itr = _tauL2.begin(); itr != _tauL2.end(); ++itr ) {

    // Should really specify the trk+calo-based 4-vector here
    float dR = goodTau.DeltaR(*(*itr)->L2); // L2Cal is the same
    if (dR < dRMin) { dRMin = dR; chain = *itr; }
  }

  if (dRMin < _maxL2dR) return chain;
  return 0;
}

const TrigTauTrigChain*
TrigTauTrigChainList::getEFChain(const TLorentzVector& goodTau) const {
  // Match EF eta/phi with visible truth

  float dRMin = 10000;
  const TrigTauTrigChain* chain(0);

  for (std::vector<const TrigTauTrigChain*>::const_iterator 
	 itr = _tauEF.begin(); itr != _tauEF.end(); ++itr ) {

    float dR = goodTau.DeltaR(*(*itr)->EF);
    if (dR < dRMin) { dRMin = dR; chain = *itr; }
  }

  if (dRMin < _maxEFdR) return chain;
  return 0;
}

const TrigTauL1Object*
TrigTauTrigChainList::getL1Tau(const TLorentzVector& goodTau) const {
  const TrigTauTrigChain* chain = getL1Chain(goodTau);
  if (chain == 0) return 0;
  return chain->L1;
}

const TrigTauL2Object*
TrigTauTrigChainList::getL2CalTau(const TLorentzVector& goodTau) const {
  const TrigTauTrigChain* chain = getL2CalChain(goodTau);
  if (chain == 0) return 0;
  return chain->L2;
}

const TrigTauL2Object*
TrigTauTrigChainList::getL2TrkTau(const TLorentzVector& goodTau) const {
  const TrigTauTrigChain* chain = getL2TrkChain(goodTau);
  if (chain == 0) return 0;
  return chain->L2;
}

const TrigTauL2Object*
TrigTauTrigChainList::getL2Tau(const TLorentzVector& goodTau) const {
  const TrigTauTrigChain* chain = getL2Chain(goodTau);
  if (chain == 0) return 0;
  return chain->L2; // L2/L2Trk/L2Cal are all the same
}

const TrigTauRecEFObject*
TrigTauTrigChainList::getEFTau(const TLorentzVector& goodTau) const {
  const TrigTauTrigChain* chain = getEFChain(goodTau);
  if (chain == 0) return 0;
  return chain->EF;
}

void
TrigTauTrigChainList::fillTriggerList(TrigTauSelectionManager& trigMan) {

  _tauL1.clear();
  _tauL2Cal.clear();
  _tauL2Trk.clear();
  _tauL2.clear();
  _tauEF.clear();
  if( _debug) cout << "Clearing is done \n";

  int count = 0;
  // Selections must be applied sequentially to the chain
  for(std::vector<TrigTauTrigChain>::const_iterator
	itr = _chainList.begin(); itr != _chainList.end(); ++itr) {
    
    if( _debug ) {cout << " analyse chain " << count << endl; }
      count++;


    // Apply L1 selection
    if (!trigMan.passedL1(*itr)) continue;
    _tauL1.push_back(&(*itr));
    if( _debug) cout << " L1 is passed \n";

    // Apply L2 selection
    bool l2CalOk = trigMan.passedL2Cal(*itr);
    if( _debug) cout << " L2Calo is "<< l2CalOk<< " \n";
    bool l2TrkOk = trigMan.passedL2Trk(*itr);
    if( _debug) cout << " L2Trk is "<< l2TrkOk<< " \n";
    if( l2CalOk ) _tauL2Cal.push_back(&(*itr));
    if( l2TrkOk ) _tauL2Trk.push_back(&(*itr));

    if (!trigMan.passedL2(*itr) ) continue;
    _tauL2.push_back(&(*itr));
    if( _debug) cout << " L2 is passed \n";

    // Put EF selection here
    if (!trigMan.passedEF(*itr)) continue;
    _tauEF.push_back(&(*itr));
    if( _debug) cout << " EF is passed \n";

  }

}

void
TrigTauTrigChainList::fillChainList(const TrigTauEvent* event) {

  _chainList.clear();

  if( event == 0 ) {
    cout << "TrigTauTrigChainList::fillChainList - No event available!" << endl;
    return;
  }

  // Check lists for consistency
  int nL1 = event->tauL1.size();
  int nL2 = event->tauL2.size();
  int nL3 = event->tauEF.size();

  // Note, this can now happen...
  static bool firstL2 = true;
  if (nL2 > nL1 && firstL2) {
    cout << "TrigTauTrigChainList::fillChainList - Found nL2>nL1 :" 
  	 << nL2 << " > " << nL1 << endl;
    cout << " last message of this type" << endl;
    firstL2 = false;
  }

  // Note, this can now happen...
  static bool firstL3 = true;
  if (nL3 > nL2 && firstL3 ){
    cout << "TrigTauTrigChainList::fillChainList - Found nEF>nL2 :" 
	 << nL3 << " > " << nL2 << ", only closest cand. with best seed will be considered " << endl;
    cout << " last message of this type\n";
    firstL3 = false;
  }
  if (_debug) cout << "nL1 " << nL1 << " nL2 " << nL2 << " nEF " << nL3 << endl;

  if (nL1 == 0) return;

  // Choose between using dR matching (old ntuples)
  // or ROI Word matching (correct, TTP05 and beyond)
  if (_useRoiWord) {
    fillChainListRoiWord(event);
  } else {
    fillChainListdRMatch(event);
  }

  // Also attach the offline tauRec and 1p3p objects
  for(unsigned int itau = 0; itau < event->nTauRec(); ++itau){
    const TrigTauRecOfflineObject * tau = &event->tauRecEntry(itau) ;
    
    float dRMin = 10000;
    std::vector<TrigTauTrigChain>::iterator closest = _chainList.end();

    for(std::vector<TrigTauTrigChain>::iterator ich = _chainList.begin(); 
	ich != _chainList.end(); ++ich) {

      // Match to highest level trigger object available
      float dR = tau->DeltaR(*(ich->trigger()));
      if (dR < dRMin) {dRMin = dR; closest = ich;}
    }

    // Asign this tauRec object to the closest chain
    if (closest != _chainList.end()) closest->tauRec = &(*tau);
  }


}

void
TrigTauTrigChainList::fillChainListdRMatch(const TrigTauEvent* event) {

  // Start with one chain per level1 object
  for(std::vector<TrigTauL1Object*>::const_iterator
	l1tau = event->tauL1.begin();
      l1tau != event->tauL1.end(); ++l1tau) {
    TrigTauTrigChain theChain;
    // cout << "New L1 " << l1tau->m_eta << endl;
    theChain.L1 = &(**l1tau);
    theChain.L1MET = &event->etMissL1;

    _chainList.push_back(theChain);
  }

  // Now loop through the L2 objects and attach each to the
  // chain with the closest L1 object in dR

  // Fix this when data is available to associate L1 to L2 by ROI ID
  for(std::vector<TrigTauL2Object*>::const_iterator
	l2tau = event->tauL2.begin(); 
      l2tau != event->tauL2.end(); ++l2tau) {

    // Check for ROI author type
    if (!(*l2tau)->m_tags.isTau_L2AuthorType(_hltAuthorType)) continue;

    float dRMin = 10000;

    // Vectors can't be 0, but lists can?  Weird
    std::vector<TrigTauTrigChain>::iterator closest = _chainList.end();

    for(std::vector<TrigTauTrigChain>::iterator ich = _chainList.begin(); 
	ich != _chainList.end(); ++ich) {

      if (ich->L1 == 0) continue;
      float dR = (*l2tau)->DeltaR(*(ich->L1));
      if (dR < dRMin) {dRMin = dR; closest = ich;}
      // cout << "L2 " << l2tau->Eta() << " " << dR << " " << dRMin << endl;
    }

    // Asign this L2 object to the closest chain
    if (closest != _chainList.end())
      { closest->L2Cal = *l2tau;  closest->L2Trk = *l2tau; closest->L2 = *l2tau;  }
  }

  //=========================================================================
  // Now loop through the EF objects and attach each to the
  // chain with the closest L2 object in dR
  for(std::vector<TrigTauTrigChain>::iterator ich = _chainList.begin(); 
      ich != _chainList.end(); ++ich) {
    if (ich->L1 == 0) continue;

    std::vector<TrigTauRecEFObject*>::const_iterator closest = event->tauEF.end();
    float dRMin = 10000;

    // look for EF candidates of type 3 first :    
    for(std::vector<TrigTauRecEFObject*>::const_iterator
	  l3tau = event->tauEF.begin(); 
	l3tau != event->tauEF.end(); ++l3tau) {
      if( (*l3tau)->m_seedType != TrigTauTauObject::BOTH ) continue;

      // Check for ROI author type
      if (!(*l3tau)->m_tags.isTau_AuthorType(_hltAuthorType)) continue;
      
      float dR = (*l3tau)->DeltaR(*(ich->L1));
      if (dR < dRMin) {
	dRMin = dR; 
	closest = l3tau;
      }
      // cout << "L3 " << l3tau->Eta() << " " << dR << " " << dRMin << endl;
    }
    
    if( dRMin <  _maxEFdR && closest != event->tauEF.end() ) {
      ich->EF = (*closest);
     continue;
    }
    
    // look if there is a better TauRec only candidate 
    for(std::vector<TrigTauRecEFObject*>::const_iterator
	  l3tau = event->tauEF.begin(); 
	l3tau != event->tauEF.end(); ++l3tau) {
      if( (*l3tau)->m_seedType != TrigTauTauObject::CALOOBJECT ) continue;

      // Check for ROI author type
      if (!(*l3tau)->m_tags.isTau_AuthorType(_hltAuthorType)) continue;
      
      float dR = (*l3tau)->DeltaR(*(ich->L1));
      if (dR < dRMin) {
	dRMin = dR; 
	closest = l3tau;
      }
      // cout << "L3 " << (*l3tau)->Eta() << " " << dR << " " << dRMin << endl;
    }
    
    if( dRMin <  _maxEFdR && closest != event->tauEF.end() ) {
      ich->EF = *closest;
     continue;
    }
    
    // now attach whatever remains from tau1p3p
    for(std::vector<TrigTauRecEFObject*>::const_iterator
	  l3tau = event->tauEF.begin(); 
	l3tau != event->tauEF.end(); ++l3tau) {
      if( (*l3tau)->m_seedType != TrigTauTauObject::TRACK ) continue;

      // Check for ROI author type
      if (!(*l3tau)->m_tags.isTau_AuthorType(_hltAuthorType)) continue;
      
      float dR = (*l3tau)->DeltaR(*(ich->L1));
      if (dR < dRMin) {
	dRMin = dR; 
	closest = l3tau;
      }
      // cout << "L3 " << l3tau->Eta() << " " << dR << " " << dRMin << endl;
    }
    
    if( closest != event->tauEF.end() ) {
      ich->EF = *closest;
     continue;
    }

  } // end of loop over chains
  
}

// We can now do this by using the m_roiWord, which should
// be a unique identifier.

void
TrigTauTrigChainList::fillChainListRoiWord(const TrigTauEvent* event) {

  // Start with one chain per level1 object
  for(std::vector<TrigTauL1Object*>::const_iterator
	l1tau = event->tauL1.begin();
      l1tau != event->tauL1.end(); ++l1tau) {
    TrigTauTrigChain theChain;
    if (_debug) cout << "New L1 - eta: " << (*l1tau)->Eta() << " ID: "
		     << std::hex << (*l1tau)->m_roiWord << std::dec << endl;
    theChain.L1 = *l1tau;
    theChain.L1MET = &event->etMissL1;

    _chainList.push_back(theChain);
  }

  // Now loop through the L2 objects and attach each to the
  // chain with the same m_roiWord

  // Fix this when data is available to associate L1 to L2 by ROI ID
  for(std::vector<TrigTauL2Object*>::const_iterator
	l2tau = event->tauL2.begin(); 
      l2tau != event->tauL2.end(); ++l2tau) {
      
    // Check for ROI author type
    if (!(*l2tau)->m_tags.isTau_L2AuthorType(_hltAuthorType)) continue;

    if (_debug) cout << "New L2 - eta: " << (*l2tau)->Eta() << " ID: "
		     << std::hex << (*l2tau)->m_roiWord << std::dec << endl;

    // Vectors can't be 0, but lists can?  Weird
    std::vector<TrigTauTrigChain>::iterator closest = _chainList.end();
      
    for(std::vector<TrigTauTrigChain>::iterator ich = _chainList.begin(); 
	ich != _chainList.end(); ++ich) {
	
      if (ich->L1 == 0) continue;
      if (ich->L1->m_roiWord == (*l2tau)->m_roiWord) closest = ich;

    }

    // Asign this L2 object to the closest chain
    if (closest != _chainList.end()) {
      closest->L2Cal = *l2tau;
      closest->L2Trk = *l2tau;
      closest->L2 = *l2tau; 
      if (_debug) cout << "Found L2 ROI Match" << endl;
    } else {
      cout << event->eventNumber << " TrigTauTrigChainList - Couldn't match L2 ROI! " << std::hex << (*l2tau)->m_roiWord << std::dec<< endl;
      if (_debug) printChainList();
    }
  }

  // Now loop through the EF objects and attach each to the
  // chain with the closest L2 object in dR
  for(std::vector<TrigTauTrigChain>::iterator ich = _chainList.begin(); 
      ich != _chainList.end(); ++ich) {
    if (ich->L2 == 0) continue;

    std::vector<TrigTauRecEFObject*>::const_iterator closest = event->tauEF.end();

    // Add bool so we get a match even if dR is crazy
    float dRMin=1E40;
    bool first = true;

    // look for EF candidates of type 3 first :    
    for(std::vector<TrigTauRecEFObject*>::const_iterator
	  l3tau = event->tauEF.begin(); 
	l3tau != event->tauEF.end(); ++l3tau) {

      if (ich->L1->m_roiWord != (*l3tau)->m_roiWord) continue;
      if( (*l3tau)->m_seedType != TrigTauTauObject::BOTH ) continue;
      
      // Check for ROI author type
      if (!(*l3tau)->m_tags.isTau_AuthorType(_hltAuthorType)) continue;

      float dR = (*l3tau)->DeltaR(*(ich->L2));
      if (first || dR < dRMin) {
	first = false;
	dRMin = dR; 
	closest = l3tau;
      }
      // cout << "L3 " << (*l3tau)->Eta() << " " << dR << " " << dRMin << endl;
    }
    
    if( closest != event->tauEF.end() ) {
      ich->EF = *closest;
     continue;
    }
    
    // look if there is a better TauRec only candidate 
    for(std::vector<TrigTauRecEFObject*>::const_iterator
	  l3tau = event->tauEF.begin(); 
	l3tau != event->tauEF.end(); ++l3tau) {
      if (ich->L1->m_roiWord != (*l3tau)->m_roiWord) continue;
      if( (*l3tau)->m_seedType != TrigTauTauObject::CALOOBJECT ) continue;

      // Check for ROI author type
      if (!(*l3tau)->m_tags.isTau_AuthorType(_hltAuthorType)) continue;
      
      float dR = (*l3tau)->DeltaR(*(ich->L2));
      if (first || dR < dRMin) {
	first = false;
	dRMin = dR; 
	closest = l3tau;
      }
      // cout << "L3 " << l3tau->Eta() << " " << dR << " " << dRMin << endl;
    }
    
    if( closest != event->tauEF.end() ) {
      ich->EF = *closest;
     continue;
    }
    
    // now attach whatever remains from tau1p3p
    for(std::vector<TrigTauRecEFObject*>::const_iterator
	  l3tau = event->tauEF.begin(); 
	l3tau != event->tauEF.end(); ++l3tau) {
      if (ich->L1->m_roiWord != (*l3tau)->m_roiWord) continue;
      if( (*l3tau)->m_seedType != TrigTauTauObject::TRACK ) continue;
      
      // Check for ROI author type
      if (!(*l3tau)->m_tags.isTau_AuthorType(_hltAuthorType)) continue;

      float dR = (*l3tau)->DeltaR(*(ich->L2));
      if (first || dR < dRMin) {
	first = false;
	dRMin = dR; 
	closest = l3tau;
      }
      // cout << "L3 " << (*l3tau)->Eta() << " " << dR << " " << dRMin << endl;
    }
    
    if( closest != event->tauEF.end() ) {
      ich->EF = *closest;
     continue;
    }
    if (_debug)
    {
      if( ich->EF != 0 )
      {
	cout << "Found EF ROI Match" << endl;
      } else {
        cout << event->eventNumber<< " TrigTauTrigChainList - Couldn't match EF ROI! "<< std::hex << ich->L1->m_roiWord << std::dec << endl;
      }
    }
  } // end of loop over chains

}

bool TrigTauTrigChainList::checkRoIWord() const
{
  bool ok = true;
  for (std::vector<TrigTauTrigChain>::const_iterator itr = _chainList.begin();  
       itr != _chainList.end(); ++itr) {
    if( ! itr->L1 ) continue;
    if( ( itr->L2 && itr->L1->m_roiWord !=  itr->L2->m_roiWord ) ||
	( itr->EF && itr->L1->m_roiWord !=  itr->EF->m_roiWord ) ||
	( itr->L2 && itr->EF && itr->L2->m_roiWord !=  itr->EF->m_roiWord ) )
      {
	ok = false;
      }
  }
  if( !ok )
    {
	cout << " MISMATCH in chain RoIs found \n";
	printChainList();
	cout << "==========================================";
    }


  return ok;
}

