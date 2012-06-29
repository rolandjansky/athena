/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage

LArGeoAlgsNV contains two key classes in the LAr GeoModel description

1. LArGeo::LArDetectorFactory builds GeoModel description of LAr calorimeter by calling
relevant 'Construction' classes (Barrel, Endcap). It also builds readout geometry
description using LArReadoutGeometry objects.

2. LArDetectorToolNV is a standard GeoModel tool, which calls LArDetectorFactory::create(),
stores LArDetectorManager to the Detector Store and also registers a callback function
align() which applies misalignments on top of the 'regular' geometry.


--------------------------------
  REQUIREMENTS 
--------------------------------

@include requirements
@htmlinclude used_packages.html 

*/
