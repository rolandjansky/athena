/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_SeededSpacePointFinderTool_page TRT_SeededSpacePointFinderTool
@author Thomas.Koffas@cern.ch

@section TRT_SeededSpacePointFinderTool_TRT_SeededSpacePointFinderToolIntro Introduction

This tool is concrete implementation for ITRT_SeeedSpacePointFinder.h
It returns a list of pairs of space points on the last two/three SCT layers.
If a single space point seed is found it will be included twice in the pair.
This tool is called by the TRT_SeeededTrackFinderTool to provide silicon seeds
for extending the initial TRT track segments into the Si ID. 

Two different implementations are provided with this package: TRT_SeededSpacePointFinderTool and SimpleTRT_SeededSpacePointFinderTool, both following different approaches to obtain the seeding pairs  

@section TRT_SeededSpacePointFinderTool_TRT_SeededSpacePointFinderToolDescription Description

The TRT_SeededSpacePointFinderTool contains the following steps:

1. Initialization step.Here the SCT space points (and also the pixel ones but only for
   debugging purposes), are read in and distributed in R-phi slises, according to their
   corresponding R-phi coordinates.A series of lists(histograms) are filled that way at
   the beginning of each new event.The coordinate system orientation is set with respect
   to the original beam direction.A cylindrical coordinate system is used throughout.

2. Based on the R-phi track parameters of the input TRT track segment, the search for Si
   seeds will proceed only in the R-phi region indicated by them and the two neighboring
   ones on either side, mostly to account for the possibility of brem losses.

3. For every space point on the last SCT layer/disc (closest to the TRT) a counterpart is searched
   for in the following one.The combination is kept only if it passes the following criteria:

   i)  The direction indicated in the R-z plane should be meaningful;

   ii) The pseudorapidity should be less than 2.5

   iii)The momentum from the curvature of the two space points and from the known field strength
       should be abobe 0.5GeV.

   iv) The estimated momentum based on the curvature indicated by the 2 space points in question and
       and the first drift circle closest to the SCT of the TRT segment under investigation, should 
       be comparable to that estimated by the TRT track segment. A chi2 is computed and a cut set via
       the job options is applied.

   v)  Similarly for the estimated theta and phi angles of the seed and the TRT segment. Again the chi2
       cuts are set via job options.

4. If no second space point is found that can be paired to the first one, then that space point is 
   returned twice inside the pair.These are the single space point seeds which for the moment are not
   processed by the tracking tools.

5. A final list of space point pair seeds is finally constructed and returned.

6. Only space points that have not already been assigned to tracks during the inside-out tracking, will
   be considered by default. agin this can be modified via job options.
     

The SimpleTRT_SeededSpacePointFinderTool follows this approach:

1) The IdentifierHash of all detector elements which are within a defined tolerance along the direction of the input Trk::TrackParameter are determined using the RegionSelectorService. These IdentifierHash are stored and define the region of interest of the seed search. The use of a defined ROI can be switched off, then all detector elements are considered for the search, with the corresponding loss of computing time.

2) All Trk::SpacePoint from collections which correspond to the IdentifierHashes in the ROI are retrieved and stored in an array of vectors of Trk::SpacePoint. The index of the array determines the SCT wheel or layer the Trk::SpacePoint are located on. Trk::SpacePoint from the SpacePointOverlapCollection are retrieved and also stored in the array, if they are located within the ROI

3) The pairs of Trk::SpacePoint are build. Only Trk::SpacePoint from different layers or wheels are paired. Which layers and/or wheels are paired is determined by a look-up table which contains all allowed combinations of wheels and layers for a given TRT ring on which the incoming Trk::TrackParameter of the TRT-segment is located. All possible combinations of Trk::SpacePoint for every allowed combination of two layers/wheels are tried, but only those are filled into the output buffer which fulfill minimum requirements on the quality of the pairing. 


@section TRT_SeededSpacePointFinderTool_TRT_SeededSpacePointFinderTools Used Tools

The TRT_SeededSpacePointFinderTool uses the IMagneticFieldTool, InDetAssociationTool tools.

The SimpleTRT_SeededSpacePointFinderTool uses the InDetAssociationTool tool, the TRT_ID and SCT_ID helpers and the RegionSelectorService.

@section TRT_SeededSpacePointFinderTool_TRT_SeededSpacePointFinderToolContents Contents

This tool contains the following implementations:

- TRT_SeededSpacePointFinder_ATL.cxx... Form SP seeds based on TRT segment track parameters.

- SimpleTRT_SeededSpacePointFinder_ATL.cxx... Form SP seeds based mainly on geometrical arguments.




*/
