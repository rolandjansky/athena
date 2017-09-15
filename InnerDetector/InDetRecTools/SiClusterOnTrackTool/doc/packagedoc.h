/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiClusterOnTrackTool_page 

@section SiClusterOnTrackTool_SiClusterOnTrackToolDocIntro Overview

This package provides two tools, PixelClusterOnTrackTool and
SCT_ClusterOnTrackTool, which turn PixelClusters
and SCT_Clusters into RIO_OnTrack-based objects, i.e.
InDet::PixelClusterOnTrack and InDet::SCT_ClusterOnTrack.

The calibration corrections which are applied to
the new objects are described below. For more information
on the concept, design and use of the <nop>RIO_OnTrack creation,
please look at the Trk::RIO_OnTrackCreator documentation.

@section SiClusterOnTrackTool_SiClusterOnTrackToolDocCorr Corrections
For <b>PixelClusters</b> the information they contain is passed to 
the RIO_OnTrack object, after applying three kinds of corrections:
<ul>
<li> A charge interpolation algorithm is applied to compute the 
pixel cluster position, provided that the cluster contains more 
than one pixel and that the flag "PositionStrategy" is greater 
than zero (see job option flags description). 
   The charge interpolation may already have been used in building 
the PixelCluster objects when raw data objects are processed.
However, the knowledge of the track candidate parameters allow the 
parameters of the charge interpolation to be better optimized.
   Since the PixelCluster position information may already take into     
account the information related to the charge, the starting point
for the charge interpolation must be recalculated from the pixel 
cells of the cluster, and it is taken as the average between the 
positions of the first and last rows (columns) for the local x (y)
direction.
   To this position, a correction dependent on the cluster size and 
the track incident angle is applied, using the variables "omegax" 
and "omegay" of the cluster, which are the charge balance between the
first and last row and columns respectively, and are numbers 
between 0 and 1. 
   The correction is Delta*(omega-0.5), where Delta is to be tuned 
according to the dimensions of the clusters and the track parameters.
In the barrel, Delta is parametrized as function of track incidence 
angle and number of rows for the local x direction, and as a function 
of pseudorapidity and the number of columns for the local y direction. 
Because of the beam spread in z, the latter parametrization is not 
ideal and should be replaced with a parametrization taking into account 
the track theta angle.
    In the pixel endcap, the correction delta is a fixed number as most 
track arrives with an incidence angle close to the normal to the silicon 
sensor, and the charge sharing only occurs in a region of limited size 
near the border between two pixel cells; Delta represents the size of this
charge sharing region, determined by diffusion of charges inside the 
silicon. It is set to 10 micrometer for collision data; for SR1 cosmics 
data, where some spread in the track incidence angle existed, a large 
value is used. 
     If PositionStrategy is set to 0, no position correction is made.
</li><br>
<li> The error associated to the position of the cluster is also 
re-computed, provided that the joboption flag "ErrorStrategy" is set 
to 2. The errors are parametrized as a function of the same 
quantities used in the parametrization of the charge interpolation 
parameters, described above.
    If ErrorStrategy is set to 1, a slightly conservative error
equal to the pixel pitch divided the square root of 12 is used. 
    If ErrorStrategy is set to 0, a very conservative error 
equal to the cluster total width divided the square root of 12 is
used. For example, this is appropriate for large clusters produced by 
an energetic secondary (ionization) electron, an hadronic interaction 
in the Pixel layer, .... 
</li><br>
<li> [In the code is also avalaible the evaluation of the effect of the distortions on the modules of the barrel.
These distortions are parametrized with the 2 curvatures (in m-1) of the module measured along the FE chip lines and a twist angle (in radians) between the 2 long sides of the modules, where the FE chip lie.
The final calculation essentially gives a function z(xloc,yloc) in the local module frame that calculate the z coordinate in the new module plane as a function of the coordinates xloc and yloc of the cluster in the nominal flat module plane (i.e. where z = 0 everywhere). This z value are finally shifted by an offset that allow to have the mean value of this z coordinate along the whole surface of the module equal to zero. This procedure avoids that alignment algorithms could correct these distortions by a rotation or a traslation of the module plane modifying the effects of these distortions.
The procedure to take into account these distortions consists then in the correction of the clusters' coordinates according with the phi and eta angles of the incident tracks w.r.t. the nominal flat module plane and with the position of the cluster in the local module frame following the function z(xloc,yloc) mentioned above.
These new clusters' coordinates are then avalaible and returned as arguments of the InDet::PixelClusterOnTrack that is the output of this class.
In the JobOptions, the user can choose between 5 options, following the value of the flag CorrectBow:
-- CorrectBow==0 means that no corrections will be applied;
-- CorrectBow==1 means that the values of the 2 curvatures R1 and R2 and the twist angle are the same for all the modules in the barrel. In particular R1=R2=0.1 m-1 and a twist angle of 0.5 mrad are the value choosen, corresponding to a sagitta of  50 microns.
-- CorrectBow==2 means that for each module R1, R2 and twist angles come from the text file PixelSurveyDistorsions.txt that contain the identifiers of each module and its values for the curvatures and twist directly coming from Survey Data . This file is avalaible in the /share directory.
-- CorrectBow==3 means that R1,R2 and twist angle are fixed, different for each module of the barrel, starting from a random gaussian generator. The mean values and RMS have been choosen from Survey Data, with the introduction of a correlation between R1 and R2 of the same module.
R1 is a random number coming from a gaussian distribution with Mean R1 = 0.12 m-1 and RMS R1 = 0.08 m-1;
R2 is the same but with Mean R2=R1 and RMS R2=0.008 m-1;
twist angle is the same with Mean Twist = 0.5 mrad and RMS Twist = 0.8 mrad.
-- CorrectBow==4 means that for each module R1, R2 and twist angles come from the Database. This options is not yet avalaible and tested. ]
</li>
</ul>

For <b>SCT_Clusters</b> error calibration is applied, too - currently
it can be set to copy the previous cluster error, define the error as
function of the strip pitch or use a tuned cluster error.

For clusters from end-cap modules the measurement is defined in the
sensor frame (already the PRD formation rotates it from strip frame
to sensor frame). However, modifications to the error affect the
<i>error in the strip frame</i>, so the code rotates internally
between the two frames.

The two-dimensional barrel SCT_Clusters are reduced to one-dimensional
measurements, i.e. the additional constraint of the hit being compatible
with the strip center at an uncertainty level of strip length/sqrt(12)
is removed. This behaviour can be restored by setting the job option
MakeTwoDimBarrelClusters=True.


@section SiClusterOnTrackTool_ExtrasSiClusterOnTrackTool Extra Pages

*/
