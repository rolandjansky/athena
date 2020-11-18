# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDataAccessMonitoring )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigDataAccessMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigDataAccessMonitoring
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthContainers AthenaKernel )

# Tests in the package:
atlas_add_test( Methods_test
                SOURCES
                test/Methods_test.cxx
                LINK_LIBRARIES TrigDataAccessMonitoringLib
                POST_EXEC_SCRIPT nopost.sh )
