/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigInDetEvent_page TrigInDetEvent Package

This package contains classes used in the LVL2 ID algorithms

@author D.Emeliyanov AT rlDOTacDOTuk

@section TrigInDetEvent_TrigInDetEventIntro Introduction

The following classes are defined

   - TrigInDetTrackFitPar : represents a set of LVL2 track parameters with associated covariance 
   - TrigInDetTrack       : a LVL2 track containing TrigInDetTrackFitPar(s) and input data - Pixel and SCT 
                            spacepoints and TRT drift circles
   - TrigSiSpacePoint     : a LVL2 spacepoint in Pixel or SCT
   - TrigVertex           : a LVL2 vertex class containing vertex coordinates, covariance matrix, and 
                            some auxilliary information about vertex
   - TrigHisto            : A base class for TrigHisto1D and TrigHisto2D, contains common code.  Not 
                            stored directly.  TP converters do not persistify this class.
   - TrigHisto1D          : A class to store a 1D histogram for a given event and provide cut
                            functionality.  This class is used by TrigTrtHitCounts.
   - TrigHisto2D          : A class to store a 1D histogram for a given event and provide cut and profile
                            functionality.  This class is used by TrigSpacePointCounts and 
                            TrigTrackCounts. 
   - TrigSpacePointCounts : Contains 3 histograms (TrigHisto2D of Pixel cluster time over threshold vs size, 
                            for endcap C, barrel and endcap A.  It also contains the total number of 
                            SCT space points for endcap C, barrel and endcap A.
   - TrigTrtHitCounts     : Contains 3 histograms (TrigHisto1D) of TRT time over threshold for
                            endcap C, barrel and endcap A.
   - TrigTrackCounts      : Contains 2 histograms (TrigHisto2D) of track z0 vs pt and eta vs phi 
                            respectively

This package also contains selection.xml - a file for lcgdict generation
      


*/
