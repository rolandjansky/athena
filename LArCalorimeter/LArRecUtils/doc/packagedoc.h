/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**

@page LArRecUtils_page LArRecUtils Package

This package provides tools for LAr reconstruction


@author Hong Ma <hma@bnl.gov>
@author Claire Adam <claire.bourdarios@cern.ch>
@author Sandrine Laplace  <laplace@lapp.in2p3.fr>
@author Kai Voss  <Kai.Voss@cern.ch>
@author G.Unal <Guillaume.Unal@cern.ch>

@section LArRecUtils_Tools for noise and OFC computation on the fly (interfaces in LArElecCalib)

LArAutoCorrTotalTool:  compute total (electronics+pileup) noise autocorrelation. 
  Returns a vector of N(N-1)/2 values with N=number of samples

LArOFCTool: tool to compute OFC on the fly, from all the basics database ingredients + using
 the LArAutoCorrTotalTool.  The OFC are computed to minimize the total noise based on the autocorrelation
  matrix given by LArAutoCorrTotalTool

The python directory contains default python classes for the configuration of these tools depending
on the job properties

@section LArRecUtils_BadChannel Tools that deal with problems

LArBadChennelTool:  classify problem channels, for H8 test beam only.


@section LArRecUtils_SignalReco Signal reconstruction tools

LArOFPeakRecoTool,LArParabolaPeakRecoTool,LArShapePeakRecoTool: signal reconstruction using different algorithms.

*/
