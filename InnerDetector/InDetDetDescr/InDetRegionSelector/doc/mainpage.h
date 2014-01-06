/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage InDetRegionSelector Package

@authors Grant Gorfine, Aline Gesualdi Mello, Seregy Sivoklokov, Mark Sutton

@section InDetRegionSelectorIntro Introduction 

This package contains code to generate the tables of element extents
used by the region selector. 

The code loops over each detector element
and fills the class RegSelSiLUT which includes for each element:
z min,
z max, 
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
and extent in z along the beamline to be used. Therefor, when a user wants
elements inside an roi, they must also specify the zmin and zmax positions
of the roi along the beamline. The default z range (DeltaZ in the old version)
is now more properly a property of the calling algorithm. 


@section InDetRegionSelectorClassOverview  Class Overview

The package contains the two tools:

  - SiRegionSelectorTable:  Pixel and SCT
  - TRT_RegionSelectorTable:  For TRT.


@section InDetRegionSelectorJobOptions  Job Options


 - InDetRegionSelectorTable_jobOptions.py: Job option fragment for initializing all tools using genConf. 
   Makes use of DetFlags to control which tables will be created.

@section InDetRegionSelectorReq Requirements

@include requirements

*/
