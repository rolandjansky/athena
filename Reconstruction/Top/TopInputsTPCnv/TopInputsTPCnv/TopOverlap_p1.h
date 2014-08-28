/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopOverlap_p1 - class to hold persistent form of TopOverlap

#ifndef TOP_OVERLAP_P1_H
#define TOP_OVERLAP_P1_H

// contains all TP ParticleLinks includes needed
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

struct TopOverlap_p1 {
  TopOverlap_p1() {;}
  ~TopOverlap_p1() {;}

  unsigned m_flag;
  unsigned m_index;
  bool m_use;

  ElementLinkIntVector_p1 m_overlapMuons_p1;
  ElementLinkIntVector_p1 m_overlapElectrons_p1;
  ElementLinkIntVector_p1 m_overlapPhotons_p1;
  ElementLinkIntVector_p1 m_overlapTauJets_p1;
  ElementLinkIntVector_p1 m_overlapJets_p1;
  ElementLinkIntVector_p1 m_overlapTracks_p1;
};

#endif // TOP_OVERLAP_P1_H
