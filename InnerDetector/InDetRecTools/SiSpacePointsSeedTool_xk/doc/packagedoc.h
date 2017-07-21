/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiSpacePointsSeedTool_xk_page SiSpacePointsSeedTool_xk
@author Igor.Gavrilenko@cern.ch
@section SiSpacePointsSeedTool_xk_SiSpacePointsSeedTool_xkIntro Introduction

This tool is concrete implementation for ISiSpacePointsSeedMaker and is devoted for space points seeds production.

SiSpacePointsSeedMaker_CTBxk provide  space points seeds production for simple CTB and cosmic run setups and
has very simple structure. This algorithm sorted all space points in radius order and take into account all two
space points combination if distance between them inside some region. For three space points seeds addition
requrement is straight line presentation in R-Z plane.
 
This algorithm uses IMagneticFieldTool tool.

SiSpacePointsSeedMaker_ATLxk provide space points seeds production for official Atlas geometry and has
more complex struture. 

First step of the algorithm is 3-dimansion ordering all space points in radius, azimuthal angle and Z-coordinate order
inside some set collections. 
After that algorithm iterate through all this collection and take into account for seeds production onle space points 
from neighbouring collection. For two space points seeds algorithm take all two space points combinations
with radial distance inside some region, dZ/dR inside some predifined cuts and Z-position after extrapolation 
to zero radis compatible with primary vertices position. If list of primary vertices is empty this Z-position
should be inside beam interaction region. Three space points seed is combination two space point seeds and
exist two addition requariments for this combination.

  1. dZ/dR both two space points seed should be the same with some tolerance.
  2. Curvature of this seed should be below some cut accordingly user request for pT min.

 This algorithm uses IMagneticFieldTool tool.


*/
