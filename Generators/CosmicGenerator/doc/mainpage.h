/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  CosmicGenerator
@author W. Seligman, M. Shapiro, I. Hinchliffe, M. Zdrazil,

@section General information

CosmicGenerator package is a generator used for the cosmic particle production.
The output will is stored in the transient event store so it can be passed to t
he simulation.

The CosmicGenerator is used e.g. by default by the G4 cosmic simulation as a so
urce of cosmic muons at ground level.

One of the features of the CosmicGenerator is the ability to filter primary muo
ns depending on their direction and energy.
If you look in jobOptions_ConfigCosmicProd.py, you will find that the following
 properties can be set:

   * CosmicGenerator.emin, CosmicGenerator.emax: energy range for the primary m
uon
    * CosmicGenerator.xvert_low, CosmicGenerator.xvert_high, CosmicGenerator.zv
ert_low, CosmicGenerator.zvert_high: the (x,z) surface at ground level in which
 the primary vertex has to be created
    * CosmicGenerator.yvert_val: the y quota at which the primary vertexes must
 be created (i.e. the "ground level")
    * CosmicGenerator.ctcut: angular cut (wrt to the vertical)

Another set of properties allows further optimization:

    * CosmicGenerator.OptimizeForCavern: if True, muons are passed to the simul
ation only if they are pointing towards the interaction point, within a given t
olerance. In order for this to work, the CosmicGenerator must be informed on wh
ere the IP is exactly. This is what the next properties are for
    * CosmicGenerator.IPx, CosmicGenerator.IPy, CosmicGenerator.IPz: the (x,y,z
) coordinates of the IP
    * CosmicGenerator.Radius: the tolerance of the direction filtering. Only mu
ons pointing inside a sphere centered in the IP with the given radius will be a
ccepted.


@htmlinclude used_packages.html

@include requirements

*/
