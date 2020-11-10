/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetRegionSelector_page InDetRegionSelector Package

@authors  Mark Sutton, Grant Gorfine, Aline Gesualdi Mello, Seregy Sivoklokov,

@section InDetRegionSelector_InDetRegionSelectorIntro Introduction

This package contains code to generate the tables of element extents
used by the region selector. 

The code loops over each detector element
and fills the class RegSelSiLUT which includes for each element:
  
  z min,
  z max,
  z2 min
  z2 max 

  r min, 
  r max,

  phi min,
  phi max,

  layer/disk number,
  barrel or endcap,
  hash Id, 
  rob Id.

The element extent in eta is no longer used, since the lookup tables are more
sophisticated and can allow elements within an roi which has a position and
and extent in z along the beamline to be used. 

Therefore, when a user wants elements inside an roi, they must *also* specify 
the zmin and zmax positions of the roi along the beamline. The default z range 
(DeltaZ in the old version) are no longer used - this now comes from each 
RoiDescriptor

The elements can be trapezoidal in r-z space, with max and min radii, and 
max and min z positions at the inner radius and z2 max and min positions
at the outer radius. If no z2 values are given, then the same z limits are 
used at both the inner and outer radius.

@section InDetRegionSelector_InDetRegionSelectorClassOverview Class Overview

The package contains no tool:

@section InDetRegionSelector_InDetRegionSelectorJobOptions Job Options


 - InDetRegionSelectorTable_jobOptions.py: Job option fragment for initializing all tools using genConf. 
   Makes use of DetFlags to control which tables will be created.



*/
