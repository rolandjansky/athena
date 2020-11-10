# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelToTPIDTool )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( PixelToTPIDTool
  PixelToTPIDTool/*.h  src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES}
  AthenaBaseComps StoreGateLib GaudiKernel TrkEventPrimitives
  AthenaPoolUtilities Identifier InDetIdentifier InDetRIO_OnTrack
  PathResolver TrkSurfaces TrkMeasurementBase TrkParameters
  TrkRIO_OnTrack TrkTrack TrkToolInterfaces PixelGeoModelLib PixelConditionsData )

# Install files from the package:
atlas_install_runtime( share/*.txt )
