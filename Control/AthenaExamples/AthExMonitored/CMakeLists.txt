# Copyright (C) 20022020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthExMonitored )

# Component(s) in the package:
atlas_add_component( AthExMonitored
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaMonitoringKernelLib)

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
