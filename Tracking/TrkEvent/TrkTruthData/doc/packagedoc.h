/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkTruthData_page TrkTruthData Package
@author Maria.Jose.Costa@cern.ch

@section TrkTruthData_TrkTruthDataIntro Introduction

This package provides the implementation of the truth classes for the different
objects of the Tracking EDM.
 
For each of the reconstructed objects (PrepRawData, SpacePoint and Track) 
there is a truth object associated with it. The truth object is meant to point 
to the source that contributed the most to the creation of that object, 
where source here stands for either noise or a generated particle. 
It contains then a HepMcParticleLink object (dominant source) and a 
probability to indicate what was the contribution of this source to the
creation of the reconstructed object.  

@section TrkTruthData_TrkTruthDataOverview Class Overview
  The MyPackage package contains of following classes:

  - PrepRawDataTruth: class to describe the truth of a given PrepRawData object
  - SpacePointTruth: class to describe the truth of a given SpacePoint object
  - TrackTruth: class to describe the truth of a given Track object


*/
