/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigTileMuId_page TrigTileMuId Package

The package provides low \f$ p_{\rm{T}} \f$ muon tagging algorithms using TileCal for LVL2 trigger.

@author Giulio Usai Giulio.Usai@cern.ch
@author Aranzazu Ruiz Martinez Aranzazu.Ruiz.Martinez@cern.ch
@author Heuijin Lim Heuijin.Lim@cern.ch

@section TrigTileMuId_TrigTileMuIdIntro Introduction

The basics of the TrigTileMuId algorithms is to look for low \f$ p_{\rm{T}} \f$ muons in the Tile Calorimeter following projective patterns in eta. The search starts from the outermost layer until the innermost layer of TileCal looking for cells with energy deposited between a low and a high energy thresholds (\f$ thr_{low} \leq  E \leq thr_{hi} \f$). The low energy threshold is meant to cut electronic noise and minimum bias pile-up events. The high energy thresholds are meant to delimit the maximum muon energy deposition while eliminating hadronic showers and tails.

@section TrigTileMuId_TrigTileMuIdOverview Class Overview

The TrigTileMuId package contains the following classes:

  - TrigTileLookForMuAlg: AllTEAlgo algorithm which performs TileCal full scan looking for low \f$ p_{\rm{T}} \f$ muons. This algorithm takes as input the energy deposited in the TileCal cells.

  - TrigTileRODMuAlg: AllTEAlgo algorithm which extracts the low \f$ p_{\rm{T}} \f$ muons tagged at the TileCal ROD DSP level. The output of the muon tagging algorithm processed at the ROD DSPs is stored in dedicated subfragments in the data format (frag types 0x10, 0x11, 0x12 and 0x13).

  - CBNT_TrigTileMu: dumps TrigTileLookForMuAlg info into CBNT directory in the ntuple for analysis.

  - CBNT_TrigTileMuROD: dumps TrigTileRODMuAlg info into CBNT directory in the ntuple for analysis.

  - TrigTileMuToNtuple: dumps TrigTileLookForMuAlg info into TileMuTag directory in the ntuple for analysis.


*/
