/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage CscClusterization package

@author dladams@bnl.gov

@section IntroductionCscClusterization Introduction

The main purpose of this package is to provide an algorithm to build CSC
(cathode strip chamber) clusters. The latter serve as input for the
construction of CSC segments or may be directly in track finding and
fitting. The currently supported algorithm is called CscThresholdClusterBuilder
and it uses a simple strategy of clustering adjacent hit strips whose total
charge is above a threshold specified in the job options.

@section ToolsCscClusterization Tools

The package also provide some supporting tools that may also be used in other
contexts. Strip fitters (interface ICscStripFitter) are used to assign the total
charge and time using the time samples associated with a strip. Cluster fitters
(interface ICscClusterFitter) calculate the position and error for a cluster.

One strip fitter is provided. CalibCscStripFitter calls the CSC calib tool and
uses its charge and time if the call is succssful. If not, it returns the peak
charge and its time. Fixed errors (values defined as job options) are returned in
either case.

Two cluster fitters are provided. SimpleCscClusterFitter assigns the position at
the center of the cluster and uses the effective with (cluster minus inherent)
to assign an error. QratCscClusterFitter use the method of adjacent charge ratios
with a lookup table to convert strip position to geometric position.

@section ConfigurationCscSegmentMakers Configuration

The tools and algorithm are configured in python. Use the following to access the default
instances:

<pre>
from CscClusterization.CalibCscStripFitter import theCalibCscStripFitter
from CscClusterization.SimpleCscClusterFitter import theSimpleCscClusterFitter
from CscClusterization.QratCscClusterFitter import theQratCscClusterFitter
from CscClusterization.CscThresholdClusterBuilder import theCscThresholdClusterBuilder
</pre>

@section DeprecatedCscClusterization Deprecated code

The orginal clusterization algorithm CscClusterBuilder is deprecated and has been
removed. Some of its supporting components, CscAverage, CscFit and CscParabola,
have been retained for possible future use.

@section AdditionalCscClusterization Additional information

 - @ref UsedCscClusterization
 - @ref RequirementsCscClusterization
 - <a href=https://twiki.cern.ch/twiki/bin/view/Atlas/CscReconstruction>CSC reconstruction overview</a>
*/

/**
@page UsedCscClusterization Used packages
@htmlinclude used_packages.html
*/

/**
@page RequirementsCscClusterization Requirements
@include requirements
*/
