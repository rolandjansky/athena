# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigOnlineMonitor )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_component( TrigOnlineMonitor
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaInterprocess AthenaKernel AthenaMonitoringKernelLib ByteStreamCnvSvcBaseLib ByteStreamData EventInfo GaudiKernel LumiBlockData MagFieldInterfaces StoreGateLib TrigConfData TrigConfL1Data TrigT1Result )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
