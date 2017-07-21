/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page AthenaServices_page AthenaServices
AthenaServices contains several ATLAS "core" components, that implement an 
interface defined in AthenaKernel or in GaudiKernel. Among those you'll find
 - the standard AthenaEventLoopMgr and its python version, plus an event loop manager  that allows multiple passes over a dataset and a MixingEventSelector that reads event from multiple input streams into a job
 - services to manage random number sequences
 - a framework to specify which data object to write out 
(AthenaOutputStream and AthenaOutputStreamTool)



@section AthenaServices_tests Unit Tests

 - AthenaOutputStream_test.cxx
 - AtRndmGen_test.cxx
 - AtRanlux_test.cxx
 - SimplePOSIXTimeKeeperSvc_test.cxx
 - SimplePOSIXTimeKeeperSvc_test.py



@section AthenaServices_refs More Documentation

More information is available from the Athena wiki 
(https://twiki.cern.ch/twiki/bin/view/Atlas/AthenaFramework).

The code can be browsed using LXR 
(http://alxr.usatlas.bnl.gov/lxr/source/atlas/Control/AthenaServices/)


@author Paolo Calafiura <Paolo.Calafiura@cern.ch>

*/
