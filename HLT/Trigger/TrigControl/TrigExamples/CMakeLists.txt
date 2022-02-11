# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigExamples )

# External dependencies:
find_package( tdaq-common COMPONENTS hltinterface )

# Component(s) in the package:
atlas_add_component( TrigExamples
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel AthenaBaseComps
                     AthenaKernel ByteStreamCnvSvcBaseLib DecisionHandlingLib TrigCompositeUtilsLib 
                     TrigPartialEventBuildingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )

# Unit tests:
atlas_add_test( MTCalibPebConfigTest
                SCRIPT python -m TrigExamples.MTCalibPebConfig
                LOG_SELECT_PATTERN "ComponentAccumulator" )
