/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetGroomer.cxx

#include "JetInterface/IJetGroomerMT.h"

//**********************************************************************

IJetGroomerMT::IJetGroomerMT() : m_ppjr(nullptr) { }

//**********************************************************************

IJetGroomerMT::~IJetGroomerMT() {
  m_ppjr = nullptr;
}

//**********************************************************************

const IJetPseudojetRetriever* IJetGroomerMT::pseudojetRetriever() const {
  return m_ppjr;
}

//**********************************************************************

void IJetGroomerMT::setPseudojetRetriever(const IJetPseudojetRetriever* ppjr) {
  m_ppjr = ppjr;
}

//**********************************************************************
