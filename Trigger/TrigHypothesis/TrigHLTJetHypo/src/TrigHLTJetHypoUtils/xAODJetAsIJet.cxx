/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"
#include <TLorentzVector.h>

using HypoJet::xAODJetAsIJet;

xAODJetAsIJet::xAODJetAsIJet(const xAOD::Jet* jet, unsigned int position):
  m_jet(jet), m_position(position){}

xAODJetAsIJet::~xAODJetAsIJet(){}

double xAODJetAsIJet::pt() const {return m_jet->pt();}

double xAODJetAsIJet::eta() const {return m_jet->eta();}

double xAODJetAsIJet::phi() const {return m_jet->phi();}

double xAODJetAsIJet::m() const {return m_jet->m();}

double xAODJetAsIJet::e() const {return m_jet->e();}

double  xAODJetAsIJet::et() const {return (m_jet->p4()).Et();}

double xAODJetAsIJet::rapidity() const {return m_jet->rapidity();}

const TLorentzVector& xAODJetAsIJet::p4() const {return m_jet->p4();}

unsigned int xAODJetAsIJet::position() const {return m_position;}

bool xAODJetAsIJet::getAttribute(const std::string &name, float& val) const{
  {return m_jet->getAttribute<float>(name, val);}
}
