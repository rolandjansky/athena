/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTEST_TLORENTZVECTORASIJET_H
#define TRIGHLTJETHYPOUNITTEST_TLORENTZVECTORASIJET_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <TLorentzVector.h>
#include "xAODJet/Jet.h"

class TLorentzVectorAsIJet: public HypoJet::IJet {
 public:
 TLorentzVectorAsIJet(const TLorentzVector& tl)
   :m_pt(tl.Et()),
    m_phi(tl.Phi()),
    m_m(tl.M()),
    m_e(tl.E()),
    m_et(tl.Et()),
    // m_eta(tl.Rapidity()), 
    m_eta(tl.Eta()), 
    m_tl(tl),
    m_position(0){
  }

  virtual double pt() const override {return m_pt;}
  virtual double eta() const override {return m_eta;}
  virtual double phi() const override {return m_phi;}
  virtual double m() const override  {return m_m;}
  virtual double e() const override  {return m_e;}
  virtual double et() const override  {return m_et;}
  virtual double rapidity() const override  {return m_eta;}
  virtual TLorentzVector p4() const override  {return m_tl;}
  virtual unsigned int position() const override {return m_position;}
  virtual bool getAttribute(const std::string&, float&) const override {return true;}
  virtual bool getAttribute(const std::string&, bool&) const override {return true;}
  virtual std::string toString() const override {return "TLorentzVectorAsIJet";}
  virtual std::optional<const xAOD::Jet*> xAODJet() const override {
    return std::optional<const xAOD::Jet*>();
  }

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
