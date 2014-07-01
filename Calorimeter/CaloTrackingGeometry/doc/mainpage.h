/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage CaloTrackingGeometry package

@author Andreas.Salzburger@cern.ch

@section CaloTrackingGeometryIntro Introduction

This packge contains two (Ath)AlgTools, the 
   - CaloTrackingGeometryBuilder
   - CaloSurfaceBuilder
   
The CaloTrackingGeometry builder retrieves sub volume builders
from the Tile and LAr calorimeter realm and combines the volumes
to one TrackingGeometry object. If a inner volume is provided it
is wrapped, if no inner volume is provided an empty one is created.

The CaloSurfaceBuilder constructs tracking surfaces that represent
cell entries or entire sampling layers in the calorimeter.


*/
