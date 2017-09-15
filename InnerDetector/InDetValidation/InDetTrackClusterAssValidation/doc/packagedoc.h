/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

@page InDetTrackClusterAssValidation_page TrackClusterAssValidation
@author Igor.Gavrilenko@cern.ch
@section InDetTrackClusterAssValidation_TrackClusterAssValidationIntro Introduction

The Algorithm TrackClusterAssValidation produces a text summary of track clusters association 
performance at the end of your Athena job. This algorithm take as input clusters, space points 
and truth collections and separate truth particles with sufficient number space points and clusters.
After that algorith compare them with clusters assosiated with tracks. Final product of this stadies
is two dimension table of probabilities to loss correct cluster and assosiate track with wrong one.


*/
