/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HadronicRecoilData/Cone.h"

// default constructor
Cone::Cone()
  :m_eta(-999),
   m_phi(-999),
   m_radius(0.),
   m_el(),
   m_mu(),
   m_trp(),
   m_hasEl(false),
   m_hasMu(false),
   m_hasTruth(false)
{
}
// main constructor
Cone::Cone(double eta, double phi, float radius)
  :m_eta(eta),
   m_phi(phi),
   m_radius(radius),
   m_el(),
   m_mu(),
   m_trp(),
   m_hasEl(false),
   m_hasMu(false),
   m_hasTruth(false)
{
}
// constructor with electron
Cone::Cone(double eta, double phi, float radius, const Analysis::Electron& el)
  :m_eta(eta),
   m_phi(phi),
   m_radius(radius),
   m_el(el),
   m_mu(),
   m_trp(),
   m_hasEl(true),
   m_hasMu(false),
   m_hasTruth(false)
{
}
// constructor with muon
Cone::Cone(double eta, double phi, float radius, const Analysis::Muon& mu)
   :m_eta(eta),
    m_phi(phi),
    m_radius(radius),
    m_el(),
    m_mu(mu),
    m_trp(),
    m_hasEl(false),
    m_hasMu(true),
    m_hasTruth(false)
{
}
// constructor with truth particle
Cone::Cone(double eta, double phi, float radius, const TruthParticle& trp)
   :m_eta(eta),
    m_phi(phi),
    m_radius(radius),
    m_el(),
    m_mu(),
    m_trp(trp),
    m_hasEl(false),
    m_hasMu(false),
    m_hasTruth(true)
{
}
// destructor
Cone::~Cone()
{
}
