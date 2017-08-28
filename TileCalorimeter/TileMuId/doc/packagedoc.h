/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileMuId_page TileMuId Package

@author Giulio Usai Giulio.Usai@cern.ch

@section TileMuId_TileMuIdIntro Introduction

This package provide muon identification exploiting  the
transverse and radial segmentation of the Tile Calorimeter.
The algorithm starts  with a search for a candidate muon in all the outer
radial layer  cells.
A candidate muon is defined when the  energy  in one cell is
 compatible with typical muon energy deposition
(i.e. \f$ thr_{low} \leq  E \leq thr_{hi} \f$).
The thresholds  cut the electronic noise, the pile up of minimum bias
events  and tails of hadronic showers.
Individual threshold for each cell are determined
using single muon events.
When a candidate is found the search is continued in   the central and
innermost layer cells  following a pattern from the cell of  candidate
 toward the interaction region, evaluating at each step the
compatibility of the cell energy  with the energy deposition of a muon.
The pattern search is implemented in the algorithm  as a look up table
which can be easily modified to improve the algorithm efficiency.
The (\f$ \eta, \phi \f$) coordinates of the found "track" are calculated
as the  average coordinates of the crossed cells in the
three layers. The typical resolutions  are
\f$ \sigma_{\eta}=0.04 \f$ and \f$ \sigma_{\phi}=0.06 \f$ rad.

@section TileMuId_TileMuIdOverview Class Overview

One simplified version of the algorithm is implemented in the ROD DSP;
this provides (online) the info described above encoded in the ROD fragment.
The TileMuId package contains the following classes:

  - TileLookForMuAlg:  algorithm which performs the muon search. This algorithm
takes as input the energy deposited in the TileCal cells.

  - CBNTAA_TileMu: dumps TileLookForMuAlg info into CBNTAA output in the ntuple for analysis.

  - TileMuToNtuple: dumps TileLookForMuAlg info into TileMuTag directory in
the ntuple for analysis.


*/
