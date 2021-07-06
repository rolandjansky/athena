# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MdtSegmentT0Fitter )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Minuit2 )

# Component(s) in the package:
atlas_add_component( MdtSegmentT0Fitter
   MdtSegmentT0Fitter/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibSvcLib
   TrkDriftCircleMath MdtCalibData MuonReadoutGeometry MuonPrepRawData
   MuonRIO_OnTrack MuonSegmentMakerInterfacesLib )
