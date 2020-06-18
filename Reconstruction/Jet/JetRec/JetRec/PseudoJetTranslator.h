// this file is  -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JETREC_PSEUDOJETTRANSLATOR_H
#define JETREC_PSEUDOJETTRANSLATOR_H

#include "xAODJet/JetContainer.h"
#include "fastjet/PseudoJet.hh"
#include "JetRec/PseudoJetContainer.h"

class PseudoJetTranslator {
public:


  PseudoJetTranslator(bool saveArea, bool saveArea4Vec) : m_saveArea(saveArea), m_saveArea4Vec(saveArea4Vec) {}
  
  xAOD::Jet& translate(const fastjet::PseudoJet& pj,
		       const PseudoJetContainer& pjCont,
		       xAOD::JetContainer& jetCont) const ;

  xAOD::Jet& translate(const fastjet::PseudoJet& pj,
		       const PseudoJetContainer& pjCont,
		       xAOD::JetContainer& jetCont,
		       const xAOD::Jet &parent) const ;

  
  
  
protected:
  bool m_saveArea = true;
  bool m_saveArea4Vec = true;
};
#endif
