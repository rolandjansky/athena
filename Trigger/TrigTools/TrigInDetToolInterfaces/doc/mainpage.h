/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrigInDetToolInterfaces Package

This package defines abstract interfaces for various LVL2 ID reconstruction tools.

@author Dmitry.Emeliyanov@cern.ch

@section TrigInDetToolInterfacesOverview Overview

The list of abstract interfaces is below:

1. Pattern recognition tools

    - ITrigHitFilter - an interface for the TrigIDSCAN HitFilter tool
    - ITrigZFinder   - an interface for the TrigIDSCAN ZFinder tool

2. Track fitting and extrapolation tools

    - ITrigInDetTrackFitter - an interface for the LVL2 ID track fitting tools
    - ITrigTRT_TrackExtensionTool - an interface for the TRT track extension tool which performs 
a simultaneous pattern recognition and track fitting in the TRT
    - ITrigInDetTrackExtrapolator - an interface for the LVL2 ID track extrapolator

3. Vertex fitting tools

   - ITrigVertexFitter - an old interface
   - ITrigL2VertexFitter - an extended inferface for the LVL2 vertex fitting algorithm

4. Data providing tools

   - ITrigTRT_DriftCircleProviderTool - an interface for the TRT data provider tool which performs ByteStream 
conversion and creates TRT RIO collections on request

*/  
