/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page egammaUtils_page egammaUtils Package

This package provides various genaral tools for egamma use, 
but also called from other packages, such as

BoostedDecisionTree 
Tool to calculate BDT - called e.g from egammaPIDTools package

FisherWrapper
TFermaAbsEngine
TFermaFisher
TFermaSample
TFermaTemplate
Tools to calculate Fisher discriminant - called e.g from egammaPIDTools package


egammaMCTruthClassifier 
Starting from electron MC truth match or a track-match, the source of electron is defined.

egammaBitMaskGenerator 
Tool to generate IsEM bitmask according to the user's requirements

egammaTrackIsolationTool
Tool to call to get track isolation: starting from CaloCluster 
Loops over TrackParticleContainer container calculating pT of the tracks which match certain 
selection criteria


@author RD Schaffer <R.D.Schaffer@cern.ch>
@author Srini Rajagopalan <srini@sun2.bnl.gov>
@author Hong Ma <hma@bnl.gov>
@author Peter Loch <loch@physics.arizona.edu>
@author Frederic Derue <derue@lpnhep.in2p3.fr>
@author Alexander Khodinov <khodinov@bnl.gov>

*/
