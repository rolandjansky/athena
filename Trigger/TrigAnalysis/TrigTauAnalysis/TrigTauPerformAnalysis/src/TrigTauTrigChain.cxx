/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTrigChain.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Container to associate L1-L2-EF into a trigger chain
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauTrigChain.h"
#include <iostream>
using namespace std;

ClassImp(TrigTauTrigChain)  

TrigTauTrigChain::TrigTauTrigChain() :
  L1(0), L2(0), L2Cal(0), L2Trk(0), EF(0), L1MET(0), tauRec(0)
{}

TrigTauTrigChain::~TrigTauTrigChain() {}

TrigTauTrigChain& 
TrigTauTrigChain::operator = (const TrigTauTrigChain& other)
{
  L1 = other.L1;
  L2Cal = other.L2Cal;
  L2Trk = other.L2Trk;
  L2 = other.L2;
  EF = other.EF;
  L1MET = other.L1MET;
  tauRec = other.tauRec;
  return *this;
}

const TLorentzVector*
TrigTauTrigChain::trigger() const {
  if (EF != 0) return EF;
  if (L2 != 0) return L2;
  if (L2Trk != 0) return L2Trk;
  if (L2Cal != 0) return L2Cal;
  return L1; // Will return null if L1 not assigned
}



void TrigTauTrigChain::print() const
{
  cout << " Chain has ";
  if( L1 ) cout << " L1 ";
  if( L2Cal ) cout << " L2Cal ";
  if( L2Trk ) cout << " L2Trk ";
  if( L2 ) cout << "L2 ";
  if( L1 && !(L2Cal or L2Trk) ) cout << " WARNING ";
  if( EF ) cout << " EF ";
  cout << endl;
  if( L1 != 0 ) 
    cout << " L1 Et(GeV) RoiWord eta phi  EmIsol: " 
	 << L1->m_TauEnergy/1000.
	 << "\t\t" << std::hex << L1->m_roiWord << " "<<std::dec << L1->Eta() << "\t" << L1->Phi() << " " << L1->m_EmIsol/1000.<<endl;
  if (L2Trk) 
    {
      cout << " L2 Et(GeV) RoiWord eta phi: " 
	   << L2Trk->Et()/1000;
      if(L2) cout << "\t" << std::hex << L2->m_roiWord;
      cout  << " "<<std::dec <<  L2Trk->Eta() << "\t" << L2Trk->Phi() 
            << std::flush;
      if( L1 && L2) cout << "\t-> dR = " << L1->DeltaR(*L2) << endl;
      else          cout << " NO L1 CASE \n";
    }
  else if (L2Cal) 
    {
      cout << " L2 Et(GeV) RoiWord eta phi: " 
	   << L2Cal->Et()/1000;
      if(L2) cout << "\t" << std::hex << L2->m_roiWord;
      cout << " "<<std::dec <<  L2Cal->Eta() << "\t" << L2Cal->Phi()
            << std::flush;
      if( L1 && L2) cout << "\t-> dR = " << L1->DeltaR(*L2) << endl;
      else          cout << " NO L1 CASE \n";
    }
  if (EF != 0) {
    cout << " EF Et(GeV) eta phi: " 
	 << EF->Et()/1000 
	 << "\t" << std::hex << EF->m_roiWord << " "<<std::dec <<  EF->Eta() << "\t" << EF->Phi()  ;
    if( L2 != 0 ) cout << "\t-> dR = " << L2->DeltaR(*EF) << endl;
    else          cout << " NO L2 CASE \n";
  }
 cout << endl;
  return ;
}

