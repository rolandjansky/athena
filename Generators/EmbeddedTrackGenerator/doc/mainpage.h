/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage EmbeddedTrackGenerator

@author Andrei Gaponenko <andrei.gaponenko@cern.ch>

EmbeddedTrackGenerator package provides provides generators that
produce single particles using reconstructed quantities from real data
events as the reference.  The generated events are intented to be
passed through GEANT simulation and overlayed with the original data
events.

The generators use a standard ATLAS POOL file (for example an AOD
file) for input event data.
 
For more details see individual generator pages of
EmbeddedTrackGenerator and VertexedTrackGenerator.

@htmlinclude used_packages.html

@include requirements

*/
