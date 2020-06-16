# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigROBDataProviderSvc )

# External dependencies:
find_package( ROOT COMPONENTS Hist )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigROBDataProviderSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigROBDataProviderSvc
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel TrigDataAccessMonitoringLib TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaMonitoringKernelLib CxxUtils StoreGateLib )

atlas_add_component( TrigROBDataProviderSvc
                     src/components/*.cxx
                     LINK_LIBRARIES TrigROBDataProviderSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( python/scripts/*.py )

