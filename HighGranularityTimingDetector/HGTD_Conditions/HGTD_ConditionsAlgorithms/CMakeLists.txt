# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_ConditionsAlgorithms )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( HGTD_ConditionsAlgorithms
                     src/*.h src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities DetDescrConditions GaudiKernel GeoModelUtilities GeoPrimitives Identifier HGTD_Identifier HGTD_ReadoutGeometry InDetReadoutGeometry StoreGateLib TrkGeometry TrkSurfaces MuonReadoutGeometry TRT_ReadoutGeometry)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
