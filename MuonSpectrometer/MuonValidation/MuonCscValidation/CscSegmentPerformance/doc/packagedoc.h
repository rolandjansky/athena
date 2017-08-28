/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page CscSegmentPerformance_page CscSegmentPerformance package

@author dladams@bnl.gov

@section CscSegmentPerformance_IntroductionCscSegmentPerformance Introduction

This package is used to check the performance of CSC segment building
by comparing the contents of the segment validation trees (see package
CscSegmentValidation) with that in the segment prediction (simseg) tree
(see CscHitValidation). The main programs csc_segment_performance
and csc_4d_segment_performance respectively match the contents of the
validation and truth trees to create the 2D and 4D segment performance trees.
The contents of the performance trees are described in the respective source
files for these executables.

The package also provides some root macros to aid in the analysis of these
trees.
 - segperf.C - macros for analyzing the 2D segment performance tree
 - seg4perf.C - macros for analyzing the 4D segment performance tree
 - report2d.C - macro to create and html report from a 2D performance tree
 - report4d.C - macro to create and html report from a 4D performance tree

@section CscSegmentPerformance_AdditionalCscSegmentPerformance Additional information

 - <a href=https://twiki.cern.ch/twiki/bin/view/Atlas/CscReconstruction>CSC reconstruction overview</a>
*/
