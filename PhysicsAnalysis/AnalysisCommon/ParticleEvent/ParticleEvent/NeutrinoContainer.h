//dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEEVENT_NEUTRINOCONTAINER_H
#define PARTICLEEVENT_NEUTRINOCONTAINER_H 1
///////////////////////////////////////////////////////////////////////////////
///
/// NAME:     NeutrinoContainer.h
/// Package : offline/PhysicsAnlaysis/AnalysisCommon/ParticleEvent
///
/// AUTHOR:  Ketevi A. Assamagan
/// CREATED:  August 2004
///
///PURPOSE:  This is a data object, containing a collection of Neutrino Objects.
///////////////////////////////////////////////////////////////////////////////

/// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "ParticleEvent/Neutrino.h"
#include "EventKernel/IParticle.h"
#include "NavFourMom/IParticleContainer.h" // needed so macros done in correct order

// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<Neutrino> derives from DV<IP>
DATAVECTOR_BASE( Neutrino, IParticle );

class NeutrinoContainer : public DataVector<Neutrino> 
{
 public:
  NeutrinoContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
	  : DataVector<Neutrino>(own) {} 
  virtual ~NeutrinoContainer() {};
  void print();

};

CLASS_DEF(NeutrinoContainer, 1215610474, 1)
SG_BASE(NeutrinoContainer, DataVector<Neutrino>);
#endif

