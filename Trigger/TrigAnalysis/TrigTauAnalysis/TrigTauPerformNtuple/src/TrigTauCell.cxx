/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauCell.h"

ClassImp(TrigTauCell) 

TrigTauCell::TrigTauCell()
  : TObject()
  , m_e(0)
  , m_eta(0)
  , m_phi(0)
  , m_samp(0)
  , m_gain(0)
  , m_noise(0)
  , m_time(0)
  , m_quality(0)
  , m_badcell(false)
{reset();}

TrigTauCell::TrigTauCell( const TrigTauCell & other)
  : TObject()
  , m_e(other.m_e)
  , m_eta(other.m_eta)
  , m_phi(other.m_phi)
  , m_samp(other.m_samp)
  , m_gain(other.m_gain)
  , m_noise(other.m_noise)
  , m_time(other.m_time)
  , m_quality(other.m_quality)
  , m_badcell(other.m_badcell)
{}

TrigTauCell::~TrigTauCell() {}
  
void TrigTauCell::reset()
{
  m_e = 0;  
  m_phi = 0;
  m_eta = 0;
  m_samp = 0;
  m_gain = 0;
  m_noise = 0;
  m_time = 0;
  m_quality = 0;
  m_badcell = false;

}
