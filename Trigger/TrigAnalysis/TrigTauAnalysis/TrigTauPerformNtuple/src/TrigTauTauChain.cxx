/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTauChain.cxx
/// Package : TrigTauPerformNtuple
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Container to associate L1-L2-EF into a trigger chain
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformNtuple/TrigTauTauChain.h"
#include <iostream>
using namespace std;

ClassImp(TrigTauTauChain)  

TrigTauTauChain::TrigTauTauChain() :
  r_L1(0), r_L2(0), r_EFlist(0), r_tracksRoIEF(0), m_tags()
				  // L2Cal(0), L2Trk(0), L1MET(0),
{}

TrigTauTauChain::~TrigTauTauChain() {reset();}

void TrigTauTauChain::reset()
{
  r_L1.Clear();
  r_L2.Clear();
  r_tracksRoIEF.Clear();
  m_tags.reset();
  r_EFlist.Clear();
}



TrigTauTauChain& 
TrigTauTauChain::operator = (const TrigTauTauChain& other)
{
  r_L1 = other.r_L1;
  r_L2 = other.r_L2;
  r_tracksRoIEF = other.r_tracksRoIEF;
  m_tags = other.m_tags;
  r_EFlist = other.r_EFlist;
  return *this;
}

const TLorentzVector*
TrigTauTauChain::trigger() const {
  const TrigTauRecEFObject* ef = EF();
  if (ef != 0) return ef;
  if (L2() != 0) return L2();
  return L1(); 
}



void TrigTauTauChain::print() const
{
  cout << " Chain has ";
  if( L1() ) cout << " L1 ";
  if( L2() ) cout << "L2 ";
  if( L1() && !L2() ) cout << " WARNING ";
  const TrigTauRecEFObject* ef = EF();
  if( ef ) cout << " EF ";
  cout << endl;
  if( L1() != 0 ) 
    cout << " L1 Et(GeV) RoiWord eta phi  EmIsol: " 
	 << L1()->m_TauEnergy/1000.
	 << "\t\t" << std::hex << L1()->m_roiWord << " "<<std::dec << L1()->Eta() << "\t" << L1()->Phi() << " " << L1()->m_EmIsol/1000.<<endl;
  if (L2() != 0 ) 
    {
      cout << " L2 Et(GeV) RoiWord eta phi: " 
	   << L2()->Et()/1000 
	   << "\t" << std::hex << L2()->m_roiWord << " "<<std::dec <<  L2()->Eta() << "\t" << L2()->Phi() ;
      if( L1() != 0 ) cout << "\t-> dR = " << L1()->DeltaR(*L2()) << endl;
      else          cout << " NO L1 CASE \n";
    }
  if (ef != 0) {
    cout << " EF Et(GeV) eta phi: " 
	 << ef->Et()/1000 
	 << "\t" << std::hex << ef->m_roiWord << " "<<std::dec <<  ef->Eta() << "\t" << ef->Phi()  ;
    if( L2() != 0 ) cout << "\t-> dR = " << L2()->DeltaR(*ef) << endl;
    else          cout << " NO L2 CASE \n";
  }
 cout << endl;
 cout << " Passing tags \n"; 
 m_tags.print();
  return ;
}

const TrigTauRecEFObject* TrigTauTauChain::EF() const 
{
  unsigned int nEF = r_EFlist.GetEntriesFast();
  for( unsigned int i=0; i<nEF; ++i)
    {
      TrigTauTauObject::SEEDTYPES type =  ((const TrigTauRecEFObject* )r_EFlist.At(i))->m_seedType;
      if( type == TrigTauTauObject::CALOOBJECT ||
	  type == TrigTauTauObject::BOTH ) 
	return (const TrigTauRecEFObject* )r_EFlist.At(i);
    }
  return 0;
}

 TrigTauRecEFObject* TrigTauTauChain::EF()  
{
  unsigned int nEF = r_EFlist.GetEntriesFast();
  for( unsigned int i=0; i<nEF; ++i)
    {
      TrigTauTauObject::SEEDTYPES type =  ((const TrigTauRecEFObject* )r_EFlist.At(i))->m_seedType;
      if(type == TrigTauTauObject::CALOOBJECT ||
	 type == TrigTauTauObject::BOTH ) 
	return ( TrigTauRecEFObject* )r_EFlist.At(i);
    }
  return 0;
}
