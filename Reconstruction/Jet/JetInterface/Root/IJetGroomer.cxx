/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetGroomer.cxx

#include "JetInterface/IJetGroomer.h"

//**********************************************************************

IJetGroomer::IJetGroomer() : m_ppjr(nullptr) { }

//**********************************************************************

IJetGroomer::~IJetGroomer() {
  m_ppjr = nullptr;
}

//**********************************************************************

const IJetPseudojetRetriever* IJetGroomer::pseudojetRetriever() const {
  return m_ppjr;
}

//**********************************************************************

void IJetGroomer::setPseudojetRetriever(const IJetPseudojetRetriever* ppjr) {
  m_ppjr = ppjr;
}

//**********************************************************************
