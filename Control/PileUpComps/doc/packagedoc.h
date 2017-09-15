/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page PileUpComps_page 

contains the Gaudi components implementing the pileup framework, in particular
the PileUpEventLoopMgr. To run a pile-up job one needs to specify that the PileUpEventLoopMgr is to be used in a "bootstrap" option file e.g.
@code
athena -p Digitization/pileUpBootstrap.py Digitization/NightlyPileUp.py
@endcode
where Digitization/pileUpBootstrap.py contains 
@code
theApp.EventLoop = "PileUpEventLoopMgr"
@endcode


@section PileUpComps_ATN test

 - Simulation/Digitization/test/TestPileUp.xml



@section PileUpComps_refs More Documentation

More information is available from the pileup wiki 
(https://twiki.cern.ch/twiki/bin/view/Atlas/PileupDigitization)

The code can be browsed using LXR 
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Control/PileUpComps/)


@author Paolo Calafiura <Paolo.Calafiura@cern.ch>

*/
