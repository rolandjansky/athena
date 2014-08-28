/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopOverlapTrigger_p1 - class to hold persistent form of TopOverlap

#ifndef TOP_OVERLAP_TRIGGER_P1_H
#define TOP_OVERLAP_TRIGGER_P1_H

// contains all TP ParticleLinks includes needed
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "TopOverlap_p1.h"

struct TopOverlapTrigger_p1 : public TopOverlap_p1 {
  TopOverlapTrigger_p1() {;}
  ~TopOverlapTrigger_p1() {;}

  // base class - using inheritance now, shouldn't need this
  //TopOverlap_p1 m_topOBase;

  bool m_inTrigger;

};

#endif // TOP_OVERLAP_TRIGGER_P1_H
