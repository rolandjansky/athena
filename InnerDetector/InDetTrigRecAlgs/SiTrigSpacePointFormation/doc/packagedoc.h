/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SiTrigSpacePointFormation_page SiTrigSpacePointFormation Package
@author Maria.Jose.Costa@cern.ch (update of the original code from David 
Candlin)

@section SiTrigSpacePointFormation_SiTrigSpacePointFormationIntro Introduction

This package is the Event Filter version of the offline
SiSpacePointFormation which contains the algorithm to create Space
Points for the Pixels and SCT detectors. For the Pixels it is just a
transformation of classes from clusters to Space Points while for the
SCT it combines the 1-dimensional information of two SCT clusters into
a Space Point.  This algorithm is used in the online Event Filter
reconstruction.

@section SiTrigSpacePointFormation_SiTrigSpacePointFormationOverview Algorithm Overview
  The SiTrigSpacePointFormation package contains the following algorithms:

   - SiTrigSpacePointFinder: It retrieves all the Pixels and SCT clusters
   and creates the SpacePoints containers: one for Pixels and two for SCT 
   non-overlapping SpacePoints. The overlapping SCT SpacePoints are saved
   in a different collection. 
   Non-overlapping means that a SpacePoint is created from 2 clusters of the 
   same module and overlapping when they are created from 2 clusters which 
   are in different but overlapping modules.
   
   The algorithm uses the classes SiElementPropertiesTable and 
   SiElementProperties in order to access, for the SCT, the backside wafer
   of a given module and the neighbour ones from other modules.


*/
