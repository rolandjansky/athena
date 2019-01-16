/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////
/// Name    : Neutrino.cxx
/// Package : offline/PhysicsAnalysis/AnalysisCommon/ParticleEvent
/// Author  : Ketevi A. Assamagan
/// Created : August 2004
/// DESCRIPTION:  construction a neutrino as an IParticle for W->l nu problems
///
//////////////////////////////////////////////////////////////////////////////////

// Header files

#include "ParticleEvent/Neutrino.h"

#include "VxVertex/RecVertex.h"

#include <math.h>

/* compiler generated defaults should be good enough

using namespace Trk;
///------------------------------------------------------------
/// Constructor -- just set defaults

Neutrino::Neutrino() :
  INavigable         ( ),
  I4Momentum         ( ),
  INavigable4Momentum( ),
  IParticle          ( ),
  P4PxPyPzEBase      ( ),
  ParticleBase       ( ),
  P4PxPyPzE          ( 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV ),
  NavigableTerminalNode()
{}

///------------------------------------------------------------
/// Copy Constructor
Neutrino::Neutrino( const Neutrino& rhs ) :
  IAthenaBarCode     ( rhs ),
  INavigable         ( rhs ),
  I4Momentum         ( rhs ),
  INavigable4Momentum( rhs ),
  IParticle          ( rhs ),
  P4PxPyPzEBase      ( rhs ),
  ParticleBase       ( rhs ),
  P4PxPyPzE          ( rhs ),
  NavigableTerminalNode( rhs )
{}

///------------------------------------------------------------
/// Assignement operator
Neutrino& Neutrino::operator=( const Neutrino& rhs )
{
  if ( this != &rhs ) {
    INavigable::operator=( rhs );
    I4Momentum::operator=( rhs );
    INavigable4Momentum::operator=( rhs );
    IParticle::operator=( rhs );
    P4PxPyPzEBase::operator=( rhs );
    ParticleBase::operator=( rhs );
    P4PxPyPzE::operator=( rhs );
    NavigableTerminalNode::operator=( rhs );
  }
  return *this;
}
*/

