/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigL2MissingET_page TrigL2MissingET Package


The TrigL2MissingET package implements the missing energy and scalar
energy sums feature extraction (Fex) algorithms for the L2 level ATLAS
trigger level.  More details are provided in the
<a href="https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasHltMetSlice">AtlasHltMetSlice</a>
twiki page.

@authors Diego Casadei <Diego.Casadei@cern.ch>
         Kyle Cranmer <cranmer@cern.ch> 
         Till Eifert <till.eifert@cern.ch> 
         Xiaowen Lei  <xiaowen@physics.arizona.edu>
         Allen Mincer <allen.mincer@nyu.edu>

@section TrigL2MissingET_TrigL2MissingETIntro Introduction

The package provides LVL2 MissingET algorithms.
It returns  etx and ety, the x and y components of missing Et, 
Et, equal to the sqrt(metx^2+mety^2), and sumEt, the scalar 
sum of the energy projected onto the transverse plane.

This is an allte algorithm, and can run in seeded or unseeded 
modes.  In the seeded mode, the roi inputs must have energy sum 
first.  LVL 2 muons can be added as a second roi.  In the 
unseeded mode only muon no roi's or only muon roi's are provided.

@section TrigL2MissingET_TrigL2MissingETOverview Class Overview

  The full work of TrigL2MissingET is currently carried out in 
  T2MissingET.cxx

@section TrigL2MissingET_TrigL2MissingETAlgorithm Algorithm properties

property                [default]                 meaning

"MissingETOutputKey"  ["T2MissingET"]     HLT feature label in navigation
"BadRegionsPhiMin" 	 empty 	          List of lower limits for dead phi regions
"BadRegionsPhiMax"       empty            List of uper limits for dead phi regions
"BadRegionsEtaMin"       empty            List of lower limits for dead eta regions
"BadRegionsEtaMax"       empty            List of uper limits for dead eta regions

As eta is not available from LVL1, the last two properties are not used.

*/
