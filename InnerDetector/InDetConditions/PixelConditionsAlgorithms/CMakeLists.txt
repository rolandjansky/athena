# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelConditionsAlgorithms )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( PixelConditionsAlgorithms
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CommissionEvent DetDescrConditions GaudiKernel GeoModelUtilities GeoPrimitives Identifier InDetCondTools InDetIdentifier InDetReadoutGeometry PathResolver PixelConditionsData PixelReadoutGeometry StoreGateLib TrkGeometry TrkSurfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
