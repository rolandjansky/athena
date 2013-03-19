/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage InDetEventCnvTool

@section InDetEventCnvTool_intro Introduction

The InDetEventCnvTool package provides the InDetEventCnvTool AlgTool, which is used by TrkTrackCollectionCnv as it recreates persistified tracks.

Specifically, it's job is to return pointers to the TrkDetElementBase and PrepRawData objects which match the passed RIO_OnTrack. The link to PrepRawData will only be recreated if 
   - the Containers are in the POOL file
   - RecreatePRDLinks = true (see below).

See the code documentation for more details.

@section InDetEventCnvTool_prop Properties
The properties declared to jobOptions are (defaults in brackets):
   - "PixelMgrLocation" : the location of the pixel ID manager ("Pixel")
   - SCT_MgrLocation" : the location of the SCT ID manager ("SCT")
   - "TRT_MgrLocation": the location of the TRT ID manager ("TRT")
   - "PixelClusterContainer" : ("PixelClusters")
   - "SCT_ClusterContainer" : ("SCT_Clusters")
   - "TRT_DriftCircleContainer" : ("TRT_DriftCircles")
   - "WriteAllTracks" : if true, write out all surfaces. Only for debugging! (false)

and most importantly:
   - "RecreatePRDLinks" : default = false. 

If this is true, then the tool will attempt to find the PrepRawData object which was used to create the RIO_OnTrack. This cannot work unless the Containers are in the POOL file (i.e. were in the Stream when the file was created), and they have the keys specified by e.g. "PixelClusterContainer".

To set a property do add the following to your jobOptions file....
ToolSvc = Service( "ToolSvc" )
InDetEventCnvTool=Algorithm( "ToolSvc.InDet::InDetEventCnvTool" )
InDetEventCnvTool.RecreatePRDLinks=TRUE

@include requirements

*/
