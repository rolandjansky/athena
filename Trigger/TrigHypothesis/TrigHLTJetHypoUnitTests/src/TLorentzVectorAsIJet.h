/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTEST_TLORENTZVECTORASIJET_H
#define TRIGHLTJETHYPOUNITTEST_TLORENTZVECTORASIJET_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <TLorentzVector.h>

class TLorentzVectorAsIJet: public HypoJet::IJet {
 public:
 TLorentzVectorAsIJet(const TLorentzVector& tl)
   :m_pt(tl.Et()),
    m_phi(tl.Phi()),
    m_m(tl.M()),
    m_e(tl.E()),
    m_et(tl.Et()),
    m_eta(tl.Rapidity()), 
    m_tl(tl),
    m_position(0){
  }
  
  double pt() const override {return m_pt;}
  double eta() const override {return m_eta;}
  double phi() const override {return m_phi;}
  double m() const override  {return m_m;}
  double e() const override  {return m_e;}
  double et() const override  {return m_et;}
  double rapidity() const override  {return m_eta;}
  const TLorentzVector & p4() const override  {return m_tl;}
  unsigned int position() const override {return m_position;}
  bool getAttribute(const std::string&, float&) const {return true;}
  std::string toString() const {return "TLorentzVectorAsIJet";}

 private:
  double m_pt;
  double m_phi;
  double m_m;
  double m_e;
  double m_et;
  double m_eta;
  TLorentzVector m_tl;
  unsigned int m_position;
};
#endif
