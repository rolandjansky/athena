/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************
//
// NAME:     TaggerHelper.cxx
// PACKAGE:  Trigger/TrigMonitoring/TrigBjetMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// *****************************************************

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrigBjetMonitoring/TaggerHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "Particle/TrackParticleContainer.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TaggerHelper::TaggerHelper(MsgStream* log) :
  m_log(log)
{}


//** ----------------------------------------------------------------------------------------------------------------- **//


TaggerHelper::~TaggerHelper() {}




