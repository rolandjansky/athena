/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkRoad_page TrkRoad Package
@author Zvi.Tarem@cern.ch

@section TrkRoad_TrkRoadIntro Introduction

The TrkRoad class encapsulates the information required by the find() method of the muon segment makers.
It contains a global position, a global direction, and the width of the road in the eta and phi directions.

Segment makers use this information in different ways:
- The DHough MDT and CSC segment makers estimate the phi position and direction of the segment from the input road.
- The DHough MDT segment maker uses the road width to limit the set of tubes used in the procedure.
- The TGC and RPC segment makers use the input direction when the provided hits are too close together and can only give an estimate of the segment position.

@section TrkRoad_TrkRoadOverview Class Overview

The class interface provides a constructor, accesss methods and stream operators for debug printouts.

The companion class TrackRoadCollection provides a persistent DataVector of TrkRoad objects. 


*/
