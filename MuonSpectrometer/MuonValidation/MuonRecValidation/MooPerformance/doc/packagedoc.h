/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MooPerformance_page The MooPerformance package

@section MooPerformance_introMooPerformance Introduction

MooPerformance is a sub-package of the Moore package.
It is intended to contain the files needed to
study the performance of muon reconstruction with
Moore and Muid. The package is interfaced with the
Run Time Tester (RTT) system to provide performance
validation of the nightly and stable releases.

@section MooPerformance_MooPerformanceSoft Software

The MooPerformance package is structured as follows:

- share: Directory containing various ROOT macros to analyze
the standard AthenaAware CBNTAA ntuple produced by RecExCommon.
In particular, macros used for detailed muon reconstruction
validation studies are included (MooPerformance.C and MooHists.C).

- test: Directory containing configuration files needed for automatic validation
in the nightlies and stable releases (e.g. RTT).

@section MooPerformance_MooPerformanceMeasures Performance Measures

Different performance measures are implemented:

- MooreQual: general track quality information is made available
  via histograms of number of tracks, chi-square, chi-square probability,
  number of eta and phi hits on track, number of stations, kinematics
  of real muon and fake tracks.

- MooreHits: hit-level histograms of number of hits in each different
  type of CSC, MDT, RPC, and TGC chambers, hit uncertainties, hit pulls,
  scattering centers.

- MooreEff: reconstruction efficiency as a function of eta and phi for
  good muons and fake tracks.

- MooreResolution: track parameter residuals and pulls distributions,
  and dependence on eta.

- The above four categories of plots are also available for Muid Standalone
  ("Standalone" prefix), Muid Combined ("Combined" prefix), and Inner
  Detector ("InnerDet" prefix).

- OverlayPlots: comparison of performance measures for the four stages
  of reconstruction.

@section MooPerformance_MooPerformanceRunning How to Run

To produce the above series of plots, one can run ROOT at the
interactive prompt:

- Update the ntuple input file names in the file MooPerformance.C.
  A single file named ntuple.root is read in by default.
  Up to four separate samples in separate ntuple files can be read in
  and processed. Plots for a given variable are then displayed for
  the four samples on the same page.

- To run in batch mode, type

  - root -q -b 'MooPerformance.C("Any_title")'

- A series of postscript files are produced containing the various plots
  described above.

@section MooPerformance_MooPerformanceSamples Samples for RTT

A series of RTT jobs are defined in the configuration file
MooPerformance_TestConfiguration.xml:

- Single-muon events with pT = 10, 100, 500 GeV/c.

- Z->mumu events with (sf05) and without cavern background.

@section MooPerformance_MooPerformanceMatch Truth Matching in MooHists

In the absence of hit-based matching between reco and truth
information, the following approaches are implemented in MooHists:

- To decide if a reconstructed MOORE muon is real or fake, we have
the option of using different algorithms. MooHists has a variable,
matchAlg, which is used to switch between the various algorithms.
At the moment, we are using matchAlg=1.
  - matchAlg = 0: Basically, any MOORE muon for which the variable rassoc is
    set to a sensible value. This occurs when the match distance
    (see next bullet) is less than 1000 mm.
  - matchAlg = 1: Reconstructed and generated MOORE muons are extrapolated
    to a common surface, and the distance between them is determined.
    We cut on this distance (function of true muon pT):
    for pT < 50 GeV/c we require Moore_matchdg < 400 mm and
    for pT > 50 GeV/c we require Moore_matchdg < 100 mm.
  - matchAlg = 2: Compare eta/phi of reconstructed and generated MOORE
    muons. Not coded as yet.
  - matchAlg = 3: Calculate a Chi-Square, using the five track parameters,
    between the reconstructed and generated MOORE muon. Waiting for errors
    to get more realistic.
  - matchAlg = 4: As in the previous case, but only use pT.
- To match a reconstructed MOORE muon to one reconstructed by Muid
  Standalone, make cuts on the eta/phi difference between the two.
  Since we have already matched a reconstructed MOORE muon to a generated
  muon, this step allows us to compare a reconstructed Standalone muon to
  the generated one.
- To match a reconstructed MOORE muon to one reconstructed by Muid Combined,
  we use a tighter cut on the eta/phi diffference between the two. Since we
  have already matched a reconstructed MOORE muon to a generated muon, this
  step allows us to compare a reconstructed Combined muon to the
  generated one.

@section MooPerformance_ExtrasMooPerformance Extra Pages


*/
