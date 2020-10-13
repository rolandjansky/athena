# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Run3AFPMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( Run3AFPMonitoring
    Run3AFPMonitoring/*.h src/*.cxx src/components/*.cxx
    INCLUDE_DIRS
        ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES
        ${ROOT_LIBRARIES}
        AthenaMonitoringLib
        AthenaMonitoringKernelLib
        LumiBlockData
        StoreGateLib
        xAODForward
        EventInfo
)

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
