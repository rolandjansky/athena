/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage The MboyPerformance package

@author samusog@cern.ch

@section introMboyPerformance Introduction

MboyPerformance is a sub-package of the Muonboy package.
It is intended to contain the files needed to
study the performance of muon reconstruction with
Muonboy, Staco and MuTag. The package is interfaced with the
Run Time Tester (RTT) system to provide performance
validation of the nightly and stable releases.

@section MboyPerformanceSoft Software

The MboyPerformance package is structured as follows:

- ExtraFiles: Directory containing various ROOT macros to analyze
the standard AthenaAware CBNTAA ntuple produced by RecExCommon.
The macros (aantana.c) reads the Athena Aware Ntuple files and produces
the efficiency, resolution, fakes,...between generated and reconstructed
tracks.  All output (png, ps, dump) files are kept.

- test: Directory containing configuration files needed for automatic
validation in the nightlies and stable releases (e.g. RTT).

@section MboyPerformanceMeasures Performance Measures

Different performance measures are implemented:

- Efficiency: reconstruction efficiency as a function of eta and phi for
  matched muons.

- Resolution: track parameter residuals and pulls distributions,
  and dependence on eta.

- Fakes

- Residual

- The above  categories of plots are also available for Staco and MuTag,
and Inner Detector.

@section MboyPerformanceMatch Truth Matching

- we first start by associating to each simulated track all the reconstructed
tracks inside a cone centered on this simulated track and of width n sigmas in
theta and phi (i.e. abs(Xrec-Xsim)/err_Xrec < n) and such that abs(1/Prec-1/Psim)/err_1/Prec < m.
If several reconstructed tracks are associated to a given simulated one, the
nearest one in angular distance (i.e. (theta_rec-theta_sim)**2 + (phi_rec-phi_sim)**2))
is kept as the match one. The default values (used for single muon events) are n = m = 50.

- all the reconstructed tracks not associated by the previous procedure are considered as fakes.

@section MboyPerformanceRunning How to Run

To produce the above series of plots, one can run ROOT at the
interactive prompt:

- Update the ntuple input file names in the file MboyPerformance.C.
  A single file named ntuple.root is read in by default.
  Up to four separate samples in separate ntuple files can be read in
  and processed. Plots for a given variable are then displayed for
  the four samples on the same page.
- Copy the file (.h), (.c) and the Makefile to a local directory, do
gmake, an executable file "ntreader" will be produce. Then you can tape
("./ntreader -h") to see the possible option. For example, if you want run
on a single ntuple, tape ("./ntreader ntuple.root")


- A series of postscript files are produced containing the variou dump
files are given containing the efficiency, the fake and the resolution
numbers described above.

@section MboyPerformanceSamples Samples for RTT

A series of RTT jobs are defined in the configuration file
MboyPerformance_TestConfiguration.xml:

- Single-muon CSC-01-02-00 events with pT = 10,100,500 GeV/c.

- Single-muon CSC-01-00-00 events with pT = 100 GeV/c.

*/

/**
@page used_MboyPerformance Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MboyPerformance Requirements
@include requirements
*/

