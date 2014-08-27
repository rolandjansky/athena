/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

 <i>

This package contains a set of algorithms that run from within Athena.
The algorithms extract the generated Monte Carlo event from the
transient event store (Storegate) and then test for various criteria.
The base class  GenFilter extracts the event from Storegate
and makes it available; all the other filter classes inherit from this.

The events are in HepMC format which is independent of the actual
Monte Carlo generator. The same filter can therefore be used with
Isajet, Herwig, Pythia  etc. Users are directed to the
ElectronFilter which illustrates the basic logic and can be used
as a template for preparing other classes. (Note that SampleFilter
is easier to read as it does not use the base class, but users
should not use SampleFilter as a template as their code will not then
be protected against changes in the way the events are held in
Storegate).

@author Ian Hinchliffe, Judith Katzy

</i>

@mainpage

@htmlinclude used_packages.html

@include requirements



*/
