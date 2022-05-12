# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree Hist )
find_package( cx_Oracle )

# Component(s) in the package:
atlas_add_library( TrigHLTMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigHLTMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaMonitoringLib GaudiKernel LumiBlockCoolQuery StoreGateLib TrigConfInterfaces TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel AthenaMonitoringKernelLib EventInfo TrigSteeringEvent xAODTrigger )

atlas_add_component( TrigHLTMonitoring
                     src/components/*.cxx 
                     LINK_LIBRARIES TrigHLTMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( run/TrigHLTMon_tf.py )
