/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page CompositeParticleEvent_page CompositeParticleEvent Package

The package which defines composite AOD particle classes.

@authors Ketevi Assamagan, Sebastien Binet, Kyle Cranmer, Hong Ma


@section CompositeParticleEvent_CompositeParticleEventIntro Introduction

AOD particle classes can be associated into another class : CompositeParticle.
This class provides an easy way to model particles which can be reconstructed from a set of other particles (<i>eg</i> a top, a \f$W^\pm\f$ or any other decaying particle).
Note that CompositeParticle implements the IParticle interface, so you have all the functionalities you are already familiar with if you know the ParticleEvent package.

@section CompositeParticleEvent_CompositeParticleEventOverview Class Overview

  The CompositeParticleEvent package contains of following classes:
 
  - CompositeParticle: it glues a vector of ParticleBase together to model a particle which can be reconstructed from its children.

  - Top: a specialisation of the CompositeParticle class

  - Boson: a specialisation of the CompositeParticle class

  - ZBoson: a specialisation of the Boson class (the PDG Id is defaulted to the good one)

  - WBoson: a specialisation of the Boson class (the PDG Id is defaulted to the good one)

@section CompositeParticleEvent_CompositeParticleEventSeeAlso See also

You may also be interested in:

   - ParticleBase

   - Analysis::JetTag and Analysis::TagInfo

   - IParticleAssocFilter : to build CompositeParticles by associating pairs of ParticleBase fulfilling a criterion

   - AnalysisUtils

   - AnalysisExamples : a set of example Algorithms. See the ttbar algorithm which exercises the composition of jets into a W as well as BJet and W into a top.



*/
