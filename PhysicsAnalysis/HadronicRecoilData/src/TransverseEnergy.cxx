/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HadronicRecoilData/TransverseEnergy.h"
#include <iostream>
//_______________________________________________________
TransverseEnergy::TransverseEnergy()
  :m_etx(0.),
   m_ety(0.),
   m_et(0.),
   m_phi(0.),
   m_etsum(0.)
{
}
//_______________________________________________________
TransverseEnergy::~TransverseEnergy()
{
}
//_______________________________________________________
TransverseEnergy::TransverseEnergy(double etx, double ety, double etsum)
  :m_etx(etx),
   m_ety(ety),
   m_et(0.),
   m_phi(0.),
   m_etsum(etsum)
{
  // et and phi are calculated from etx and ety
  SetEtPhi();
}
//_______________________________________________________
TransverseEnergy::TransverseEnergy( const TransverseEnergy& transEn )
  :m_etx   (transEn.etx()),
   m_ety   (transEn.ety()),
   m_et    (transEn.et()),
   m_phi   (transEn.phi()),
   m_etsum (transEn.etsum())
{
}
//_______________________________________________________
void TransverseEnergy::Add( double etx, double ety, double etsum, double scaleFactor )
{
  m_etx   += scaleFactor*etx;
  m_ety   += scaleFactor*ety;
  m_etsum += scaleFactor*etsum;

  // et and phi are calculated
  SetEtPhi();
}
//_______________________________________________________
void TransverseEnergy::Add( double etx, double ety )
{
  Add( etx, ety, TMath::Sqrt( etx*etx + ety*ety ), 1.);
}
//_______________________________________________________
void TransverseEnergy::Add( const TransverseEnergy& transEne, double scaleFactor )
{
  Add( transEne.etx(), transEne.ety(), transEne.etsum(), scaleFactor ); 
}
//_______________________________________________________
void TransverseEnergy::SetEtPhi()
{
  m_et    = TMath::Sqrt( m_etx*m_etx + m_ety*m_ety );
  m_phi   = std::atan2(m_ety, m_etx); 
}


