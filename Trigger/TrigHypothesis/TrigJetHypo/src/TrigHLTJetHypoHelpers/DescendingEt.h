/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_DESCENDINGET_H
#define TRIGJETHYPO_DESCENDINGET_H

// ********************************************************************
//
// NAME:     DescendingEt.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "xAODJet/Jet.h"

struct DescendingEt:
  public std::binary_function<const xAOD::Jet*, const xAOD::Jet*, bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r) const;
};

#endif
