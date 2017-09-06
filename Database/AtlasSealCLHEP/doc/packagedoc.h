/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AtlasSealCLHEP_page AtlasSealCLHEP Package

This package contains Reflex dictionary definitions for some CLHEP classes used within ATLAS.

@author R.D.Schaffer@cern.ch, Marcin.Nowak@cern.ch

@section AtlasSealCLHEP_IntroductionAtlasSealCLHEP Introduction

CLHEP classes used in ATLAS are defined in this package. This package
was originally needed when CLHEP classes were being written out
directly. This has mostly changed with the transient/persistent
separation so that this dictionary is only needed for python
interaction with CLHEP classes.

There is also some ROOT streamers classes which allow reading old data
with schema evolved CLHEP classes in the data. This is no longer
needed with data written with from release 13 and beyond.

*/
