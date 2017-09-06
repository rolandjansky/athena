/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page egammaTrackTools_page egammaTrackTools Package

This package provides various tools related to track for egamma use.

egammaTrkRefitterTool : It recives a egamma object or Trk::Track and refits the track associated with an electron. NOTE: a new track has been created to ensure that no memory leaks occur you must delete the Trk::Track pointed to by this function.

BremsstrahlungHelix : Fit a track using the calorimeter cluster as hit measurement in the hope of ID a brem

BremsstrahlungHelixOld : Calulate the Phi position of track given a set of starting parameters and a destination radius or Z position. This code is not used anymore.

BremsstrahlungHit :General hit definition used for Bremsstrahlung

EMAmbiguityTool : This tool resolves the electron-converted photon ambiguity

EMExtrapolationTools : Tool which proagates a track the calorimeter/cluster.

@author F. Derue <derue@lpnhe.in2p3.fr>

*/
