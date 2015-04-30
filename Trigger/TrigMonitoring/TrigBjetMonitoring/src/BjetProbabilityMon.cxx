/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************
//
// NAME:     BjetProbabilityMon.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Ariel Schwartzman / Andrea Coccaro
// EMAIL:    sch@slac.stanford.edu / Andrea.Coccaro@ge.infn.it
// 
// *************************************************************

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrigBjetMonitoring/BjetProbabilityMon.h"
#include "TrigBjetMonitoring/TaggerHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "Particle/TrackParticleContainer.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


BjetProbabilityMon::BjetProbabilityMon(HLTBjetMonTool* bjetMon, MsgStream* log) :
  m_bjetMon(bjetMon)
{

  m_taggerHelper = new TaggerHelper(log);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


BjetProbabilityMon::~BjetProbabilityMon() {

  if (m_taggerHelper) delete m_taggerHelper;
}


