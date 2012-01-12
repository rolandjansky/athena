/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauJet.h"

#include <cmath>

ClassImp(TrigTauJet )

TrigTauJet::TrigTauJet()
{
  reset();
}

TrigTauJet::~TrigTauJet()
{}

void TrigTauJet::reset()
{
  SetPtEtaPhiM(0,0,0,0);
  m_weight =0; 
  
  m_emJESfactor = 0;
  
  m_jetn90cells = 0;
  m_jetn90constituents = 0;
  m_jetQuality = 0;
  m_jetTimeClusters = 0;
  m_jetTimeCells = 0;
  m_jetEMf = 0;
  m_jetHECf = 0;
  m_jetTG3f = 0;
  m_jetCorf = 0;
}

bool TrigTauJet::isGood() const
{
  return !isBad() && !isUgly();
}

bool TrigTauJet::isBad() const
{
  if( m_jetHECf > 0.8 && m_jetn90cells <= 5)
      return true;
  if( m_jetEMf > 0.95 && std::fabs( m_jetQuality)>0.8)
      return true;
  if( std::fabs( m_jetTimeCells) >= 50)
      return true;

  return false;
}

bool TrigTauJet::isUgly() const
{
  if( m_jetCorf>0.5)
      return true;
  if( m_jetTG3f > 0.5)
      return true;

  return false;
}
