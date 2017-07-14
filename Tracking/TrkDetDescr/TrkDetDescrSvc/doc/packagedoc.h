/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkDetDescrSvc_page TrkDetDescrSvc Package

This Package contains the Trk::TrackingGeometrySvc and its calibariton version for the ATLAS Tracking Geometry
The geometry building is triggered through a callback from the EventInfo to be able to update the geometry
for misalignment, etc.

@author Andreas.Salzburger@cern.ch

@section TrkDetDescrSvc_TrkDetDescrSvcIntro Introduction

There are two services that can be used :

   - TrackingGeometrySvc: the standard tracking geometry service that steers the building
     of the TrackingGeometry, the association of the material and the recording to the
     detector store.
     
   - CalibrationTrackingGeometrySvc allows to apply additional distortions, material scaling.



*/
