# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackValidation )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore )

# Component(s) in the package:
atlas_add_component( InDetTrackValidation
   InDetTrackValidation/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel
   AtlasDetDescr CxxUtils EventPrimitives GaudiKernel InDetIdentifier InDetPrepRawData
   InDetRIO_OnTrack InDetRawData InDetReadoutGeometry InDetTrackSplitterToolLib SCT_CablingLib
   SCT_ConditionsToolsLib StoreGateLib TrkMeasurementBase TrkSpacePoint TrkToolInterfaces TrkTrack xAODEventInfo )
