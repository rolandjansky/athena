/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page CscSegmentMakers_page CscSegmentMakers package

@author dladams@bnl.gov

@section CscSegmentMakers_IntroductionCscSegmentMakers Introduction

This packge provides algorithms for constructing CSC (cathode strip chamber)
segments. There are two algorithms: Csc2DSegmentMaker constructs 2D segments
(1D position and 1D direction) from clusters and Csc4dSegmentMaker constructs
4D segments (2D position and 2D direction) from 2D segments.

@section CscSegmentMakers_ConfigurationCscSegmentMakers Configuration

The algorithms are configured in python. Use the following to access the default
instances:
<pre>
from CscSegmentMakers.Csc2dSegmentMaker import theCsc2dSegmentMaker
from CscSegmentMakers.Csc4dSegmentMaker import theCsc4dSegmentMaker
</pre>

@section CscSegmentMakers_AdditionalCscSegmentMakers Additional information

 - <a href=https://twiki.cern.ch/twiki/bin/view/Atlas/CscReconstruction>CSC reconstruction overview</a>
*/
