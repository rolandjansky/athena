/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// this is somewhat ridiculous.

#include "SGTools/BaseInfo.h"
#include "Navigation/IAthenaBarCode.h"
#include "Particle/TrackParticle.h"
#include "EventKernel/INavigable4Momentum.h"

SG_ADD_BASE( Rec::TrackParticle, SG_VIRTUAL (IAthenaBarCode) ); 
SG_ADD_BASE( Rec::TrackParticle, SG_VIRTUAL (INavigable4Momentum) ); 
