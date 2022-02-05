# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_ConditionsAlgs )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( COOL COMPONENTS CoolKernel CoolApplication )

# Component(s) in the package:
atlas_add_component( TRT_ConditionsAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES LINK_LIBRARIES ${CORAL_LIBRARIES} ${COOL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities DetDescrConditions GaudiKernel GeoModelUtilities GeoPrimitives InDetIdentifier StoreGateLib TRT_ConditionsData TRT_ConditionsServicesLib TRT_ReadoutGeometry )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
