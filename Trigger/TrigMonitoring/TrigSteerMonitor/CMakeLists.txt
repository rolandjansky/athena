# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSteerMonitor )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist )

# Interface library
atlas_add_library( TrigSteerMonitorLib
                   INTERFACE
                   PUBLIC_HEADERS TrigSteerMonitor
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( TrigSteerMonitor
                     src/*.h src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaInterprocess AthenaKernel AthenaMonitoringKernelLib GaudiKernel TrigCompositeUtilsLib TrigConfData TrigCostMonitorMTLib TrigSteerMonitorLib xAODEventInfo xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
