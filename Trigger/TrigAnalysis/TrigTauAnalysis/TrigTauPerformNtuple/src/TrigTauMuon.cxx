/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigTauPerformNtuple/TrigTauMuon.h"
ClassImp(TrigTauMuon )

TrigTauMuon::TrigTauMuon()
  :  TLorentzVector(0,0,0,0)
  , m_charge(0)
  , m_EtCone02(0)
  , m_EtCone30(0)
  , m_EtCone40(0)
 , m_author(0)

{}

TrigTauMuon::TrigTauMuon(const TrigTauMuon & other)
  : TLorentzVector(other)
  , m_charge   (other.m_charge )
  , m_EtCone02 (other.m_EtCone02)
  , m_EtCone30 (other.m_EtCone30)
  , m_EtCone40 (other.m_EtCone40)
 , m_author(other.m_author)
 {}

 TrigTauMuon::~TrigTauMuon() {}
 void TrigTauMuon::reset()
{
  SetPtEtaPhiM(0,0,0,0);
  m_charge =0; 
  m_EtCone02 =0; 
  m_EtCone30 =0; 
  m_EtCone40 =0; 
  m_author = 0;
  
}

