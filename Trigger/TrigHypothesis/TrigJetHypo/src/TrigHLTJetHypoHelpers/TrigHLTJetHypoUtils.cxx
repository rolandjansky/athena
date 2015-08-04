/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h"

/* WORK IN PROGRESS _ DO NOT USE ! */

Condition::Condition(double etaMin, double etaMax, double threshold): 
  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){}

bool Condition::etaOK(const xAOD::IParticle* ip) const noexcept {
  if (std::abs(ip->eta()) < m_etaMin){return false;}
  if (std::abs(ip->eta()) >= m_etaMax){return false;}
  return true;
}

bool Condition::etOK(const xAOD::IParticle* ip) const noexcept {
  if ((ip->p4()).Et() < threshold()){return false;}
  return true;
}

bool Condition::allOK(const xAOD::IParticle* ip) const noexcept {
  return etaOK(ip) && etOK(ip);
}

double Condition::etaMin() const noexcept {return m_etaMin;}
double Condition::etaMax() const noexcept {return m_etaMax;}
double Condition::threshold() const noexcept {return m_threshold;}

//////////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os, const Condition& c){
  return os << "Condition: etaMin "
            << c.etaMin() 
            << " etaMax " 
            << c.etaMax() 
            << " threshold: " 
            << c.threshold()
            <<'\n';
}


bool ConditionsSorter::operator()(const Condition& l, const Condition& r){
  return l.threshold() > r.threshold();
}


