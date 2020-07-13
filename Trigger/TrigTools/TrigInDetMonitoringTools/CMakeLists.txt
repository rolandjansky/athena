# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetMonitoringTools )

# External package dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in this package:
atlas_add_component( TrigInDetMonitoringTools
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib xAODTrackingCnvLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
