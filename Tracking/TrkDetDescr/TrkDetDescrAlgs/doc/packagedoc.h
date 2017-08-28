/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkDetDescrAlgs_page TrkDetDescrAlgs Package

This package contains validation and material association algorithms for
the TrackingGeometry.

    - The Trk::MaterialStepAssociation Algorithm is designed to map Geantino maps onto the
      static TrackingGeometry layers. The material is recorded using the Trk::MaterialMapper
      from the TrkDetDescrTools package.

    - The Trk::TrackingGeometryValidation Algorithm records the TrackingGeometry material
      with the Trk::MaterialMapper AlgTool to be compared with the output from the 
      Trk::MaterialStepAssociation Algorithm.

    - The Trk::GeometryBuilderTest is a simple test algorithm for the creation of the 
      TrackingGeometry.

    - The Trk::TrkDetDescrAthenaPoolTest checks reading and writing of TrkDetDescr EDM
      to Pool files.    

@author Andreas.Salzburger@cern.ch

@section TrkDetDescrAlgs_TrkDetDescrToolsIntro Introduction




*/
