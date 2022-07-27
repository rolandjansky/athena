/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBTAGEMULATION_JET_H
#define TRIGBTAGEMULATION_JET_H

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include <string>

namespace Trig {

class TrigBtagEmulationJet {
public:
  TrigBtagEmulationJet() = delete;
  TrigBtagEmulationJet(const xAOD::Jet&, 
		       const std::string& btagLink = "btaggingLink");
  virtual ~TrigBtagEmulationJet() = default;
  
  double pt() const;
  double eta() const;
  double phi() const;
  float jvt() const;
  const TLorentzVector& p4() const;
  const xAOD::Jet* jet() const;
  const xAOD::BTagging* btag() const;
  
  bool satisfy(const std::string& tagger_name,
	       double workingPoint) const;
  
private:
  double dl1r_weight(double pu, double pb, double pc) const;
  
private:
  const xAOD::Jet *m_jet = nullptr;
  const xAOD::BTagging *m_btag = nullptr;
  
  double m_pt;
  double m_eta;    
  double m_phi;
  double m_dl1r_cFrac = 0.018;
  TLorentzVector m_p4;
};

 inline double TrigBtagEmulationJet::pt() const { return m_pt; }
 inline double TrigBtagEmulationJet::eta() const  { return m_eta; };
 inline double TrigBtagEmulationJet::phi() const { return m_phi; }
 inline float TrigBtagEmulationJet::jvt() const {
   static const SG::AuxElement::Accessor<float> JVT( "Jvt" );
   return JVT(*m_jet);
 }
 inline const TLorentzVector& TrigBtagEmulationJet::p4() const { return m_p4; }
 inline const xAOD::Jet* TrigBtagEmulationJet::jet() const { return m_jet; }
 inline const xAOD::BTagging* TrigBtagEmulationJet::btag() const { return m_btag; }
 inline double TrigBtagEmulationJet::dl1r_weight(double pu, double pb, double pc) const 
 { return std::log( pb / ((pu * (1 - m_dl1r_cFrac)) + (m_dl1r_cFrac * pc)) ); }
 
} //namespace

#endif
