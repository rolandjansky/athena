/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     BjetLikelihoodMon.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrigBjetMonitoring/BjetLikelihoodMon.h"
#include "TrigBjetMonitoring/TaggerHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "Particle/TrackParticleContainer.h"

#include "TMath.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


BjetLikelihoodMon::BjetLikelihoodMon(HLTBjetMonTool* bjetMon, MsgStream* log) :
  m_bjetMon(bjetMon)
{
  
  m_taggerHelper = new TaggerHelper(log);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


BjetLikelihoodMon::~BjetLikelihoodMon() {

  if (m_taggerHelper) delete m_taggerHelper;
}


