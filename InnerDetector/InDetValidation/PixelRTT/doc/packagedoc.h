/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PixelRTT_page PixelRTT Package

@author Simone.Montesano@cern.ch

@section PixelRTT_PixelRTTIntro Introduction

This package is intended as a container for various Pixel-specific validation tasks that are automatically run by the RTT.
It basically includes a job option and some root classes and macros plus the xml file for the RTT job configuration.

The joboption that is used to run InDetTrackValidation tools during the recostruction from digit to AOD in order to crate an Ntuple containing residuals, pulls and other pixel related quantities usefull to create validation histograms.
It also enables the creation of the histograms contained in the PixelMonitoring package.

The root macros defined in the package are useful to easily create multiple histograms containing the same quantities for every layer, clustersize, eta and p_t bins of the Pixel detector.
The macro included create a default set of this kind of histograms and are automatically executed by the RTT.



*/
