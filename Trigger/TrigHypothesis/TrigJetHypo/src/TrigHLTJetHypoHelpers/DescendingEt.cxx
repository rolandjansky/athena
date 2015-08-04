/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DescendingEt.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include "./DescendingEt.h"


bool DescendingEt::operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
  return l->p4().Et() > r->p4().Et();
}

