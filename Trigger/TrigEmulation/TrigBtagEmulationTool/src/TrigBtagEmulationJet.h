/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBTAGEMULATION_JET_H
#define TRIGBTAGEMULATION_JET_H

#include "AthenaBaseComps/AthMessaging.h"

#include "xAODJet/JetContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include <string>

namespace Trig {

class TrigBtagEmulationJet :
    public AthMessaging {
public:
  TrigBtagEmulationJet() = delete;
  TrigBtagEmulationJet(const std::string& name, const xAOD::Jet*);
  TrigBtagEmulationJet(const std::string& name, const xAOD::JetRoI*, bool isJJ = false);
  TrigBtagEmulationJet(const TrigBtagEmulationJet&) = delete;
  TrigBtagEmulationJet& operator=(const TrigBtagEmulationJet&) = delete;
  virtual ~TrigBtagEmulationJet() = default;
  
  double pt() const;
  double eta() const;
  double phi() const;
  float jvt() const;
  
  bool isPassedBTagger(const std::string& btagger, 
		       double workingPoint) const;
  
private:
  double dl1r_weight(double pu, double pb, double pc) const;
  bool satisfy(const std::string& tagger_name,
	       const xAOD::BTagging *btag,
	       double workingPoint) const;
  
private:
  const xAOD::Jet *m_jet = nullptr;
  
  double m_pt;
  double m_eta;    
  double m_phi;
  double m_dl1r_cFrac = 0.018;
};

 inline double TrigBtagEmulationJet::pt() const { return m_pt; }
 inline double TrigBtagEmulationJet::eta() const  { return m_eta; };
 inline double TrigBtagEmulationJet::phi() const { return m_phi; }
 inline float TrigBtagEmulationJet::jvt() const {
   static const SG::AuxElement::Accessor<float> JVT( "Jvt" );
   return JVT(*m_jet);
 }
 inline double TrigBtagEmulationJet::dl1r_weight(double pu, double pb, double pc) const 
 { return log( pb / ((pu * (1 - m_dl1r_cFrac)) + (m_dl1r_cFrac * pc)) ); }
 
} //namespace

#endif
