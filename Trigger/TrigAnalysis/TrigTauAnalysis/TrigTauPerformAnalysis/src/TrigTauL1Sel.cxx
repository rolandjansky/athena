/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL1Sel.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L1 selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TrigTauPerformAnalysis/TrigTauL1Sel.h"

#include "TrigTauPerformNtuple/TrigTauL1Object.h"

#include <math.h>
#include <iostream>
using namespace std;


TrigTauL1Sel::TrigTauL1Sel() : cut_EmEnergy(0), cut_TauEnergy(0),
			       cut_EmIsol(0),cut_HdIsol(0), 
			       cut_EmCore(0), cut_HdCore(0) , _debug(false) ,
			       m_histManager(0)
{}

TrigTauL1Sel::TrigTauL1Sel(const TrigTauL1Sel& other) : 
  cut_EmEnergy( other.cut_EmEnergy  ), 
  cut_TauEnergy(other.cut_TauEnergy ),
  cut_EmIsol(   other.cut_EmIsol    ),
  cut_HdIsol(   other.cut_HdIsol    ), 
  cut_EmCore(   other.cut_EmCore    ), 
  cut_HdCore(   other.cut_HdCore    ), 
  _debug(other._debug),
  m_histManager   (other.m_histManager)
{}


TrigTauL1Sel::~TrigTauL1Sel()
{
  if( m_histManager ) { delete m_histManager; m_histManager=0;}
}

void
TrigTauL1Sel::setCuts(float emE, 
		     float tauE, 
		     float EmI, 
		     float HdI, 
		     float EmCore, 
		     float hdCore) {
  cut_EmEnergy = emE;
  cut_TauEnergy = tauE;
  cut_EmIsol = EmI;
  cut_HdIsol = HdI;
  cut_EmCore = EmCore;
  cut_HdCore = hdCore;
}

void TrigTauL1Sel::removeEmIsol()
{
  cut_EmIsol = 0;
}


void
TrigTauL1Sel::print() const {
  cout << " LVL1 cuts :" << endl;
  if (cut_EmEnergy > 1.e-10) {
    cout << " EmEnergy  > "<<cut_EmEnergy  <<endl;
  } else {
    cout << " EmEnergy:   not applied"  <<endl;
  }

  if (cut_TauEnergy > 1.e-10) {
    cout << " TauEnergy > "<<cut_TauEnergy <<endl;
  } else {
    cout << " TauEnergy:  not applied" << endl;
  }

  if (cut_EmIsol > 1.e-10) {
    cout << " EmIsol    <= " << cut_EmIsol    <<endl;
  } else {
    cout << " EmIsol:     not applied" << endl;
  }

  if (cut_HdIsol > 1.e-10) {
    cout << " HdIsol    <= " << cut_HdIsol    <<endl;
  } else {
    cout << " HdIsol:     not applied" << endl;
  }

  if (cut_EmCore > 1.e-10) {
    cout << " EmCore    > "<<cut_EmCore    <<endl;
  } else {
    cout << " EmCore:     not applied" << endl;
  }

  if (cut_HdCore > 1.e-10) {
    cout << " HdCore    > "<<cut_HdCore    <<endl;
  } else {
    cout << " HdCore:     not applied" << endl;
  }
}

bool
TrigTauL1Sel::passed(const TrigTauL1Object& tau) const {
  return (cutWord(tau) == 0); 
}

void TrigTauL1Sel::setHistDebug(TrigTauHistogram::HISTTYPE type)
{
  if( type != TrigTauHistogram::NONE ){
    //gROOT->cd();
    if( m_histManager ) delete m_histManager;
    m_histManager = new TrigTauHistogram(type);
    m_histManager->book("baseL1","Before any cut");
    m_histManager->book("EmEnergy","L1: Cut on EmEnergy");
    m_histManager->book("TauEnergy","L1: Cut on TauEnergy (ET)");
    m_histManager->book("EmIsol","L1: Cut on EmIsol");
    m_histManager->book("HadIsol","L1: Cut on  HadIsol");
    m_histManager->book("EmCore","L1: Cut on EmCore");
    m_histManager->book("HadCore","L1: Cut on HadCore");
  }else{
      if( m_histManager ) { delete m_histManager; m_histManager=0;}
  } 
}




// L1 used the convention that E > Ecut for thresholds
// and EIso <= EIsoCut for isolation. 
// For this reason, energy == cut will fail.
// Return bit pattern showing cuts failed

// L1 used the convention that E > Ecut for thresholds
// and EIso <= EIsoCut for isolation. 
// For this reason, energy == cut will fail.
unsigned long 
TrigTauL1Sel::cutWord(const TrigTauL1Object& tau) const {

  float varHist = 0;
  if( m_histManager)
    {
      if( m_histManager->type() == TrigTauHistogram::PTHIST ) varHist = tau.m_TauEnergy;
      else if( m_histManager->type() == TrigTauHistogram::ETAHIST ) varHist = tau.Eta();
      else if( m_histManager->type() == TrigTauHistogram::PHIHIST ) varHist = tau.Phi();
    }
  if( m_histManager) m_histManager->fill(0, varHist);

  unsigned long word = 0;

  if( _debug )cout << " L1:  eta="<<tau.Eta() <<", phi="<<tau.Phi() << " RoIWord="
		   <<  std::hex <<tau.m_roiWord << std::dec <<endl;

  if( _debug )cout <<" L1 candidate EmEnergy="<< tau.m_EmEnergy<< ", TauEnergy="<< tau.m_TauEnergy
		   <<", EmIsol="<< tau.m_EmIsol<< "\n HdIsol="<<tau.m_HdIsol<<", EmCore="
		   << tau.m_EmCore<<", HdCore="<<tau.m_HdCore<<endl;

  if( cut_EmEnergy  >1.e-10 && cut_EmEnergy  >= tau.m_EmEnergy ){
    if( _debug )cout<<"     -- EmEnergy reject \n";
    word |= L1CUT_EMENER;
  }
  if( (word == 0) && m_histManager) m_histManager->fill(1, varHist);

  if( cut_TauEnergy >1.e-10 && cut_TauEnergy >= tau.m_TauEnergy) {
    if( _debug )cout<<"     -- TauEnergy reject \n";
    word |= L1CUT_TAUCLUS;
  }
  if( (word == 0) && m_histManager) m_histManager->fill(2, varHist);

  if( cut_EmIsol    >1.e-10 && cut_EmIsol    < tau.m_EmIsol) {
    if( _debug )cout<<"     -- EmIsol reject \n";
    word |= L1CUT_EMISOL;
  }
  if( (word == 0) && m_histManager) m_histManager->fill(3, varHist);

  if( cut_HdIsol    >1.e-10 && cut_HdIsol    < tau.m_HdIsol) {
    if( _debug )cout<<"     --HdIsol  reject \n";
    word |= L1CUT_HDISOL;
  }
  if( (word == 0) && m_histManager) m_histManager->fill(4, varHist);

  if( cut_EmCore    >1.e-10 && cut_EmCore    >= tau.m_EmCore) {
    if( _debug )cout<<"     -- EmCore reject \n";
    word |= L1CUT_EMCORE;
  }
  if( (word == 0) && m_histManager) m_histManager->fill(5, varHist);

  if( cut_HdCore    >1.e-10 && cut_HdCore    >= tau.m_HdCore) {
    if( _debug )cout<<"     -- HdCore reject \n";
    word |= L1CUT_HDCORE;
  }
  if( (word == 0) && m_histManager) m_histManager->fill(6, varHist);
  
  if( _debug && (word == 0)) cout << "   -- accept\n";
  return word;
}


