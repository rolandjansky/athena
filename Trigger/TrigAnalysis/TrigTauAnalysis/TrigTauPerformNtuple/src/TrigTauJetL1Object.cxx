/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauJetL1Object.h"
ClassImp(TrigTauJetL1Object )


TrigTauJetL1Object::TrigTauJetL1Object()
  :  TLorentzVector(0,0,0,0)
  ,  m_eta(0)
  ,  m_phi(0)
  ,  m_ET4x4(0)
  ,  m_ET6x6(0)
  ,  m_ET8x8(0)
{}

TrigTauJetL1Object::TrigTauJetL1Object(const TrigTauJetL1Object & other)
  : TLorentzVector(other),
    m_eta( other.m_eta),
    m_phi( other.m_phi ),
    m_ET4x4( other.m_ET4x4),
    m_ET6x6( other.m_ET6x6 ),
    m_ET8x8( other.m_ET8x8)
{}

TrigTauJetL1Object::~TrigTauJetL1Object(){}

void TrigTauJetL1Object::reset()
{
   SetPtEtaPhiM(0,0,0,0); 
    m_eta   = 0;
    m_phi   = 0;
    m_ET4x4 = 0;
    m_ET6x6 = 0;
    m_ET8x8 = 0;
}
