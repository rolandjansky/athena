# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1NSW )

# External dependencies:
find_package( ROOT COMPONENTS Tree )

# Component(s) in the package:
atlas_add_component( TrigT1NSW
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib CxxUtils EventInfo GaudiKernel MuonRDO TrigInterfacesLib TrigT1NSWSimToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

