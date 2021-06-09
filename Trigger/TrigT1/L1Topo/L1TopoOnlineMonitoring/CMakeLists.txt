# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoOnlineMonitoring )

# External dependencies
find_package( tdaq-common COMPONENTS eformat )

# Components in the package:
atlas_add_component( L1TopoOnlineMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS
                     ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES
                     ${TDAQ-COMMON_LIBRARIES}
                     GaudiKernel AthenaKernel AthenaMonitoringKernelLib ByteStreamCnvSvcBaseLib
                     L1TopoRDO TrigT1Result TrigT1Interfaces xAODTrigger DecisionHandlingLib
                     TrigCompositeUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
