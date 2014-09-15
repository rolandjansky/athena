/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage CscClusterPerformance package

@author dladams@bnl.gov

@section IntroductionCscClusterPerformance Introduction

This package is used to check the performance of CSC cluster building
by comparing the contents of the cluster validation tree (see package
CscClusterValidation) with those from the cluster prediction (simpos)
tree (see CscHitValidation). The main program csc_cluster_performance
constructs a new tree by matching the contents of these two trees. The contents
of the new tree are described in the source file for this executable.

The package also provides some root macros to aid in the analysis of these
trees.
 - cscperf.C - macros for analyzing the cluster performance tree
 - report.C - macro to create an HTML report from a tree

@section AdditionalCscClusterPerformance Additional information

 - @ref UsedCscClusterPerformance
 - @ref RequirementsCscClusterPerformance
 - <a href=https://twiki.cern.ch/twiki/bin/view/Atlas/CscReconstruction>CSC reconstruction overview</a>
*/

/**
@page UsedCscClusterPerformance Used packages
@htmlinclude used_packages.html
*/

/**
@page RequirementsCscClusterPerformance Requirements
@include requirements
*/
