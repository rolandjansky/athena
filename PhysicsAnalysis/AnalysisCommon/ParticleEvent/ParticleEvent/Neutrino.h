// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEEVENT_NEUTRINO_H
#define PARTICLEEVENT_NEUTRINO_H 1
//////////////////////////////////////////////////////////////////////////////
/// Name    : Neutrino.h
/// Package : offline/PhysicsAnalysis/AnalysisCommon/ParticleEvent
/// Author  : Ketevi A. Assamagan
/// Created : August 2004
///
/// DESCRIPTION: neutrino class for t -> bW -> b l nu
///              construction the neutrino after solving the for the neutrino pz
///              using the W mass constraint. There is a quadratic ambiguity
///               leading to up to 2 solutions 
/// This class inherits from IParticle, I4Momentum and NavigableTerminalNode. 
///
////////////////////////////////////////////////////////////////////////////////

#include "ParticleEvent/ParticleImpl.h"
#include "FourMom/Lib/P4ImplPxPyPzE.h"
#include "Navigation/Navigable.h"

namespace Trk { class RecVertex; }

typedef NavigableTerminalNode NeutrinoNavigation;

class Neutrino : public ParticleImpl<NeutrinoNavigation,
				     P4ImplPxPyPzE>
{
  typedef ParticleImpl<NeutrinoNavigation,
		       P4ImplPxPyPzE> NeutrinoImpl_t;
 public:

  /** Destructor
   */
  ~Neutrino() = default;


 private:

};

#endif // PARTICLEEVENT_NEUTRINO_H

