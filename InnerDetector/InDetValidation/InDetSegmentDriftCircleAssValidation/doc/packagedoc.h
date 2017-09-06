/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetSegmentDriftCircleAssValidation_page InDetSegmentDriftCircleAssValidation
@author Thomas.Koffas@cern.ch

@section InDetSegmentDriftCircleAssValidation_InDetSegmentDriftCircleAssValidationIntro Introduction

This is an algorithm used to associate TRT segments to truth particle
segments in order to provide an efficiency estimation. The association
is done on a hit-by-hit basis.

@section InDetSegmentDriftCircleAssValidation_InDetSegmentDriftCircleAssValidationDescription Description

This algorithm performs the following tasks:

1.  Loads the reconstructed TRT segments, the container of the truth
    associated TRT PRDs and the container of all the reconstructed TRT
    drift circles.

2.  Loops over all the reconstructed TRT drift circles and using their
    associated truth particle, creates all truth particles track segments
    inside the TRT. Cuts on pT, eta, origin of truth particle and minimum
    number of drit circles, all configurable via job options, are applied
    in order to select the reconstructable truth TRT track segments. These
    are stored in a multimap. The rea possibility of a TRT drift circle
    been shared by more than one truth particle is taken into account.

3.  Then loops over the reconstructed TRT segments and extracts the drift
    circles from each one of them. These are used to decide on what truth
    particle to associate to the reconstructed track. A minimum number of
    correct hits, again configurable via the job options, is required in
    order for the assignment to be valid.

4.  By comparing the results of setps 2,3 above an efficiency is computed.

@section InDetSegmentDriftCircleAssValidation_InDetSegmentDriftCircleAssValidationContents Algorithm Contents

The InDetSegmentDriftCircleAssValidation contains the following classes/files:

- InDetSegmentDriftCircleAssValidation.cxx...Main class that does the truth association and computes an efficiency.




*/
