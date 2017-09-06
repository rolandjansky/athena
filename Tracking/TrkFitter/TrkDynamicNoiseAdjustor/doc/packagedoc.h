/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkDynamicNoiseAdjustor_page The TrkDynamicNoiseAdjustorInterfaces package

@author vakhtang.kartvelishvili@cern.ch

@section TrkDynamicNoiseAdjustor_introductionTrkDynamicNoiseAdjustor Introduction
TrkDynamicNoiseAdjustor is a tool used by the TrkKalmanFitter, which can be used
to improve momentum reconstrcution for bremming electron tracks.

@section TrkDynamicNoiseAdjustor_howitworksTrkDynamicNoiseAdjustor How it works:
TrkDynamicNoiseAdjustor works by adding appropriately calculated noise term to the
Trk::qOverP element
of the covariance matrix during filtering. This noise term is calculated
dynamically, whenever a local single-parameter fit suggests that a
significant momentum loss has taken place at a given silicon layer.



*/
