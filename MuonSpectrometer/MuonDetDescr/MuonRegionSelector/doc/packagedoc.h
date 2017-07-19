/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonRegionSelector_page MuonRegionSelector Package

@authors Mark Sutton, Stefania Spagnolo

@section MuonRegionSelector_MuonRegionSelectorIntro Introduction

This package contains code to generate the tables of element extents
used by the region selector for the Muon system

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
rob Id - this entry isn't currently used by the Muon system

The element extent in eta is no longer used, since the lookup tables are more
sophisticated and can allow elements within an roi which has a position and
and extent in z along the beamline to be used. Therefor, when a user wants
elements inside an roi, they must also specify the zmin and zmax positions
of the roi along the beamline. The default z range (DeltaZ in the old version)
is now more properly a property of the calling algorithm. 


@section MuonRegionSelector_MuonRegionSelectorClassOverview Class Overview

The package contains the two tools:

  - RPC_RegionSelectorTable:  RPC
  - TGC_RegionSelectorTable:  TGC
  - MDT_RegionSelectorTable:  MDT
  - CSC_RegionSelectorTable:  CSC


@section MuonRegionSelector_MuonRegionSelectorJobOptions Job Options


 - MuonRegionSelectorTable_jobOptions.py: Job option fragment for initializing all tools using genConf. 
   Makes use of DetFlags to control which tables will be created.



*/
