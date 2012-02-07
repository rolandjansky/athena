/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage ParticleTruth

@author Andreas.Wildauer@cern.ch

@section introduction Introduction:
This package provides truth (association) classes for the TrackParticle in Particle.
Files were moved from the Particle package to here.

@section packagecontent Package Contents:
ParticleTruth contains the following files/classes:
- TrackParticleTruth.cxx ... contains a HepMcParticle which provides a link to a GenParticle from MC truth.
- TrackParticleTruthCollection.cxx ... container for the TrackParticleTruth objects. It does not inherit from DataVector< TrackParticle > but from map. The key of the map is a pointer to a TrackParticle with which you can retrieve the associated TrackParticleTruth object.
- TrackParticleTruthCollectionContainer ... container of the TrackParticleTruthCollection used in the trigger navigation.
- TrackParticleTruthVector.cxx ... an object necessary for custom converters (ignore it).

@section used_packagesParticleTruth used packages
@htmlinclude used_packages.html

@section requirements Requirements
@include requirements

@namespace Rec Namespace of the ParticleTruth package.
*/
