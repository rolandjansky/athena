/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iostream>
using namespace std;

#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformNtuple/TrigTauEvent.h"

ClassImp(TrigTauMCTauList) 

const bool _debug=false;

TrigTauMCTauList::TrigTauMCTauList(const TrigTauMCEvent* mcEvent) :
  _selHad(true),
  _sel1pr(false),
  _sel3pr(false),
  _selMode(TrigTauMCTau::NONE),
  _minVisPt(10.),
  _maxVisEta(2.5),
  _minLeadPt(0.),
  _recoMatchdR(0.2),
  _offlineSelection(TAUCUTSAFEMEDIUM),
  _offlineCriteria(OFFLINEREL14)
 {
   // Make sure the lists start empty
   _mcTauPure.clear();
   _mcTauGood.clear();

   // Fill if we have a pointer
   if (mcEvent != NULL) {
     fillPureList(mcEvent);
     fillGoodList();
   }

}

TrigTauMCTauList::~TrigTauMCTauList() {}

float TrigTauMCTauList::nominalMinVisPt(TrigTauFlags::TRIGGERMENU menuTau)
{
  switch (menuTau ){
  case TrigTauFlags::TRK9: 
  case TrigTauFlags::TRK9_ID: 
  case TrigTauFlags::TRK_CALIB:
    return 9.;
  case TrigTauFlags::TAU12:
    return 15.;
  case TrigTauFlags::TAU16:
  case TrigTauFlags::TRK16_ID:
  case TrigTauFlags::TAU16_MEDIUM:
  case TrigTauFlags::TAU16_TIGHT:
    return 20.;
  case TrigTauFlags::TAU20_MEDIUM:
  case TrigTauFlags::TAU20_MEDIUM1:
  case TrigTauFlags::TAU20_TIGHT:
  case TrigTauFlags::TAU20:
    return 25.;
  case TrigTauFlags::TAU29:
  case TrigTauFlags::TAU29_MEDIUM:
  case TrigTauFlags::TAU29_MEDIUM1:
  case TrigTauFlags::TAU29_TIGHT:
    return 35.;
  case TrigTauFlags::TAU38_MEDIUM:
  case TrigTauFlags::TAU38_TIGHT:
  case TrigTauFlags::TAU38:
    return 45.;
  case TrigTauFlags::TAU50_MEDIUM:
  case TrigTauFlags::TAU50_TIGHT:
  case TrigTauFlags::TAU50:
    return 60.;
  case TrigTauFlags::TAU84_MEDIUM:
  case TrigTauFlags::TAU84_TIGHT:
  case TrigTauFlags::TAU84:
    return 100.;
  case TrigTauFlags::TAU100_MEDIUM:
  case TrigTauFlags::TAU100_MEDIUM1:
    return 125.;
  case TrigTauFlags::TAU125_MEDIUM:
  case TrigTauFlags::TAU125_TIGHT:
  case TrigTauFlags::TAU125:
    return 150.;
  default : cout << " TrigTauMCTauList::nominalMinVisPt ERROR  do not know which min pt to set for " 
		 <<  TrigTauFlags::menuName(menuTau) << endl; return 0;
  }
  return 0.;
}

void TrigTauMCTauList::setMinVisPt(TrigTauFlags::TRIGGERMENU menu)
{
  setMinVisPt(nominalMinVisPt(menu));
}



void 
TrigTauMCTauList::printPureList() const
{
  if (_mcTauPure.size() == 1) {
    cout << " There  is " << _mcTauPure.size() << " tau\n";
  } else {
    cout << " There are " << _mcTauPure.size() << " taus\n";
  }

  for(std::vector<TrigTauMCTau>::const_iterator itr = _mcTauPure.begin();
      itr != _mcTauPure.end(); ++itr) {

    cout << " Found tau with ID " << itr->pdgId() 
	 << " from " << (itr->mother() ? itr->mother()->pdgId() : 0) << endl;
    
    unsigned int ndau = itr->nSavedDaughters();
    for(unsigned int idau=0; idau<ndau; idau++) {
      const TrigTauTrueParticle* dau = itr->daughter(idau);
      cout << "     --> " 
	   << dau->pdgId()  << " " 
	   << dau->status() << " " 
	   << dau->E() << endl;
    }
    
  }

  // Additionally dump out the start of the MC listing?
//   for(std::vector<TrigTauTrueParticle>::const_iterator itau = 
// 	mcEvent->particleList.begin();
//       itau != mcEvent->particleList.end(); ++itau )
//     {
      
//       // Only print out the status=3 stuff
//       if ((itau->status()!=3)&&(abs(itau->mother()->pdgId())!=15)) continue;
      
//       cout << " pdgId:  " << itau->pdgId()
// 	   << " Mother: " << (itau->mother()==0 ? 0 : itau->mother()->pdgId())
// 	   << " Status: " << itau->status()
// 	   << " NDau:   " << itau->nDaughters()
// 	   << endl;
      
//     }

}

void TrigTauMCTauList::fillPureList(const TrigTauMCTauList & otherList)
{
  if (_debug) cout << "Starting fillList" << endl;
  _mcTauPure.clear();
  _mcTauGood.clear();
 
  unsigned int nTau = otherList.nMCTauPure();
  _mcTauPure.reserve(nTau);
  for(unsigned int itau = 0; itau < nTau ; ++ itau)
    _mcTauPure.push_back(otherList.mcTauPureEntry(itau));
  
  return;
}


void
TrigTauMCTauList::fillPureList(const TrigTauMCEvent* mcEvent) 
{
  if (_debug) cout << "Starting fillPureList" << endl;
  if( mcEvent == 0 ) {
    cout << "TrigTauMCTauList::fillPureList - No event available!" << endl;
    return;
  }

  _mcTauPure.clear();
  _mcTauGood.clear();

  // Find taus and remove duplicates from the MC particle listing

  TrigTauTrueParticle* mother=0;
  for(std::vector<TrigTauTrueParticle*>::const_iterator itau = 
        mcEvent->particleList.begin();
      itau != mcEvent->particleList.end(); ++itau )
    {
      if (_debug) cout << "Particle ID: " << (*itau)->pdgId() << endl;

      // Is this a tau?
      //      cout << "pdgId: " << (*itau)->pdgId() << endl;
      if (abs((*itau)->pdgId()) != 15) continue;
      mother = 0;

      if (_debug) cout << "Found a tau" << endl;

      // First save ultimate mother
      if ((*itau)->mother() != 0 )
	{
	  //	  cout << "mother pdgId: " << (*itau)->mother()->pdgId() << endl;
	  if( abs((*itau)->mother()->pdgId()) != 15) 
	    mother = (TrigTauTrueParticle*) (*itau)->mother();
	  else if( (*itau)->mother()->mother() != 0 )
	    mother = (TrigTauTrueParticle*) (*itau)->mother()->mother();	    
	}

      if (_debug && mother != 0) cout << "   and found its mother with ID " << 
				   mother->pdgId() << endl;

      // Skip record-keeping entries
      //      cout << "status: " << (*itau)->status() << endl;
      if ((*itau)->status() == 3) continue;

      if (_debug) cout << "Found status tau with " << (*itau)->nDaughters() 
		       << " daughters and " << (*itau)->nSavedDaughters() 
		       << " saved daughters" << endl;

      // Second, skip tau->tau decays
      //      cout << "nDaugh: " << (*itau)->nDaughters() << endl;
      if ((*itau)->nDaughters() < 2) continue;
      if ((*itau)->nSavedDaughters() < 2) continue;  // hack for 5103?
      if (_debug && (*itau)->nDaughters() == 2) {
	cout << "Daughter 1: " << (*itau)->daughter(0)->pdgId() << endl;	
	cout << "Daughter 2: " << (*itau)->daughter(1)->pdgId() << endl;
      }

      // Also skip tau->gamma tau decays (does photos make tau g g?)
      if ((*itau)->nDaughters() == 2 && 
          (abs((*itau)->daughter(0)->pdgId()) == 15 || 
           abs((*itau)->daughter(1)->pdgId()) == 15)) continue;

      // OK, this should be a good tau.  Create a copy and update the pdgMother
      if (_debug) cout << "Found good tau, push back" << endl;

      TrigTauMCTau theTau(**itau);
      if( mother != 0 ) theTau.setMother(*mother);

      _mcTauPure.push_back(theTau);

    }

  return;
}

  // Now, go through the pure list and apply fiducial cuts      
void
TrigTauMCTauList::fillGoodList() 
{
  _mcTauGood.clear();

  for(std::vector<TrigTauMCTau>::iterator 
        itau = _mcTauPure.begin();
      itau != _mcTauPure.end(); ++itau ) {
    if( isGoodTau( *itau) ) _mcTauGood.push_back(&(*itau));
  }
  
  
  return;

}

unsigned int TrigTauMCTauList::nMCTauPure() const
{
  return _mcTauPure.size();
}
    
unsigned int TrigTauMCTauList::nMCTauGood() const
{
  return _mcTauGood.size();
}

const TrigTauMCTau & TrigTauMCTauList::mcTauPureEntry(unsigned int i ) const
{
  return _mcTauPure[i];
}
const TrigTauMCTau & TrigTauMCTauList::mcTauGoodEntry(unsigned int i ) const
{
  return *(_mcTauGood[i]);
}

//
// Apply fiducial cuts to true taus
//
bool
TrigTauMCTauList::isGoodTau(TrigTauMCTau& tau) const {

  //
  // Hadronic decay cut
  //
  if (_selHad && !tau.hadronicDecay()) return false;

  //
  // One or 3 prong cut
  //
  if (_sel1pr && !tau.isOneProng()) return false;
  if (_sel3pr && tau.isOneProng()) return false;

  //
  // Decay mode cut (if enabed)
  //

  if ((_selMode != TrigTauMCTau::NONE) &&
      (tau.decayType() != _selMode)) return false;

  //
  // Parent ID cut.  Allow W, Z, H, but not gamma
  //

  if (tau.mother() && abs(tau.mother()->pdgId()) < 23 &&abs(tau.mother()->pdgId())>9 ) return false;
  //if (abs(tau.mother()->pdgId()) > 37) return false;

  //
  // Fiducial cuts on visible momenta
  //
  if (fabs(tau.visible().Eta()) > _maxVisEta) return false;
  if (_minVisPt > 1.e-5 && tau.visible().Pt() < 1000.*_minVisPt) return false; 
  if (_minLeadPt> 1.e-5 && tau.leadingTrack() && tau.leadingTrack()->Pt() < 1000.*_minLeadPt ) return false;

  return true;
}

void TrigTauMCTauList::requireRecoMatch(const TrigTauEvent& event, 
					OFFLINESELECTION selection, OFFLINECRITERIA criteria) {
  setRecoSelection(selection, criteria);
  requireRecoMatch(event);
}

void TrigTauMCTauList::requireRecoMatch(const TrigTauEvent& event){
    
  if( _offlineSelection ==  NONE ) return; // nothing to do


  // Loop over mcTauGood vector and remove any entries which
  // don't have an offline reco match

  std::vector<TrigTauMCTau*>::iterator itr = _mcTauGood.begin() ;
  while( itr != _mcTauGood.end() ){
    //for (itr = _mcTauGood.begin(); itr != _mcTauGood.end(); ++itr) {
    TrigTauMCTau* tau = *itr;

    // 
    // Offline reco match
    //
    bool matched = false;
    bool foundTau1p3p = false;
    bool foundTauRec = false;

    //=================================================
    for (unsigned int irec=0; irec<event.nTauRec(); irec++) {
      const TrigTauRecOfflineObject * tauRec = & event.tauRecEntry(irec);
      if( tau->DeltaR(*tauRec) > _recoMatchdR) continue;
      
      if(  tauRec->m_seedType == TrigTauTauObject::CALOOBJECT ||
	    tauRec->m_seedType == TrigTauTauObject::BOTH ){
	if(  _offlineCriteria == OFFLINEREL13 ){
	  if( tauRec->m_LikelihoodOld > 4 ) foundTauRec = true;
	}else{
	  foundTauRec = tauRec->goodTauRec();
	}
      } // end of tauRec ID

      if(  tauRec->m_seedType == TrigTauTauObject::TRACK ||
	    tauRec->m_seedType == TrigTauTauObject::BOTH ){

	if(  _offlineCriteria == OFFLINEREL13 ) {
	  if( tauRec->m_discriCut>0.5 && 
	      tauRec->leadingTrack() && tauRec->leadingTrack()->Pt() >9000 )
	    foundTau1p3p = true;
	}else{
	  foundTau1p3p = tauRec->goodTau1p3p();
	}
 
      if (_offlineSelection == TAUCUTSAFELOOSE && tauRec->isTauCutSafeLoose()) matched = true;
      else if ( _offlineSelection == TAUCUTSAFEMEDIUM && tauRec->isTauCutSafeMedium()) matched = true;
      else if ( _offlineSelection == TAUCUTSAFETIGHT && tauRec->isTauCutSafeTight()) matched = true;
      else if ( _offlineSelection == TAULLHLOOSE && tauRec->isTauLlhLoose()) matched = true;
      else if ( _offlineSelection == TAULLHMEDIUM && tauRec->isTauLlhMedium()) matched = true;
      else if ( _offlineSelection == TAULLHTIGHT && tauRec->isTauLlhTight()) matched = true;
      else if ( _offlineSelection == SAFELLHLOOSE && tauRec->isSafeLlhLoose()) matched = true;
      else if ( _offlineSelection == SAFELLHMEDIUM && tauRec->isSafeLlhMedium()) matched = true;
      else if ( _offlineSelection == SAFELLHTIGHT && tauRec->isSafeLlhTight()) matched = true;

      } // end of tauRec ID
      
    } // end of tauID


    switch (  _offlineSelection ) {
    case TAU1P3P :
      if( foundTau1p3p )  matched = true; break;
    case  TAUREC:
      if( foundTauRec )  matched = true; break;
    case  EITHERTAU:
      if( foundTau1p3p ||foundTauRec  )  matched = true; break;
    case  BOTHTAU:
      if( foundTau1p3p && foundTauRec )  matched = true; break;
    case  NONE: 
    case TAUCUTSAFELOOSE: break;
    case TAUCUTSAFEMEDIUM: break;
    case TAUCUTSAFETIGHT: break;
    case TAULLHLOOSE: break;
    case TAULLHMEDIUM: break;
    case TAULLHTIGHT: break;
    case SAFELLHLOOSE: break;
    case SAFELLHMEDIUM: break;
    case SAFELLHTIGHT: break;
    default : 
      matched = true;
      break;
    }


    if (!matched) {
      // Delete this entry
      itr = _mcTauGood.erase(itr);      
    }else{
      itr++;
    }
  } // end of while

}

const TrigTauMCTau*
TrigTauMCTauList::leadingPtGoodTau() const {

  const TrigTauMCTau* leading = 0;
  std::vector<TrigTauMCTau*>::const_iterator iGood;
  for (iGood = _mcTauGood.begin(); iGood != _mcTauGood.end(); iGood++) {
    if (leading == 0) {
      leading = *iGood;
    } else if (leading->visible().Pt() < (*iGood)->visible().Pt()) {
      leading = *iGood;
    }
  }

  return leading;
}

const TrigTauMCTau*
TrigTauMCTauList::nextPtGoodTau() const {

  const TrigTauMCTau* leading = 0;
  const TrigTauMCTau* next = 0;

  std::vector<TrigTauMCTau*>::const_iterator iGood;
  for (iGood = _mcTauGood.begin(); iGood != _mcTauGood.end(); iGood++) {
    if (leading == 0) {
      leading = *iGood;
    } else if (leading->visible().Pt() < (*iGood)->visible().Pt()) {
      leading = *iGood;
    } else if (next == 0) {
      next = *iGood;
    } else if (next->visible().Pt() < (*iGood)->visible().Pt()) {
      next = *iGood;
    }
  }

  /*
  if (leading == 0) {
    cout << "Didn't find a leading tau" << endl;
  } else {
    cout << "Found a leading tau ";
    if (next == 0) {
      cout << "didn't find next" << endl;
    } else {
      cout << "and the next" << endl;
    }
  }
  */

  return next;
}


void TrigTauMCTauList::printSettings() const
{
  cout << "TrigTauMCTauList selects true taus with: \n";
  switch (_selMode){
  case TrigTauMCTau::ENUNU: cout << " enunu"; break;
  case TrigTauMCTau::MUNUNU : cout << " mununu"; break;
  case TrigTauMCTau::RHONU : cout << " rhonu"; break;
  case TrigTauMCTau::PINU : cout << " pinu"; break;
  case TrigTauMCTau::PR3 : cout << " 3-pr"; break;
  case TrigTauMCTau::KNU : cout << " Knu"; break;
  case TrigTauMCTau::OTHERPR1 : cout << " other 1pr"; break;
  case TrigTauMCTau::OTHER : cout << " other "; break;
  case TrigTauMCTau::NONE : cout << " all"; break;
    
  }
  if( _selHad ) cout << " hadronic "; else cout << " all ";
  if( _sel1pr ) cout << " 1-prong";
  if( _sel3pr ) cout << " 3-prong";
  cout << " decays with \npT > " << _minVisPt << " GeV, |eta| < " << _maxVisEta 
       << " and leading particle pT > " << _minLeadPt << endl;
  cout << " offline candidate is ";
  switch (_offlineSelection){
  case TAU1P3P : cout << " good tau1p3p" ; break;
  case TAUREC : cout << " good tauRec" ; break;
  case EITHERTAU : cout << " good tauRec OR tau1p3p" ; break;
  case BOTHTAU : cout << " good tauRec AND tau1p3p " ; break;
  case TAUCUTSAFELOOSE: cout << " good TauCutSafeLoose" << endl; break;
  case TAUCUTSAFEMEDIUM: cout << " good TAUCUTSAFEMedium" << endl; break;
  case TAUCUTSAFETIGHT: cout << " good TAUCUTSAFETight" << endl; break;
  case TAULLHLOOSE: cout << " good TauLlhLoose" << endl; break;
  case TAULLHMEDIUM: cout << " good TauLlhMedium" << endl; break;
  case TAULLHTIGHT: cout << " good TauLlhTight" << endl; break;
  case SAFELLHLOOSE: cout << " good SafeLlhLoose" << endl; break;
  case SAFELLHMEDIUM: cout << " good SafeLlhMedium" << endl; break;
  case SAFELLHTIGHT: cout << " good SafeLlhTight" << endl; break;
 
  case NONE : cout << " NOT required" ; break;
  }
  cout <<" in dR < " << _recoMatchdR << endl;
}
