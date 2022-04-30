# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1CaloFEXByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS  eformat_write  )

# Component(s) in the package:
atlas_add_component( L1CaloFEXByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES TrigT1ResultByteStreamLib xAODTrigger ${TDAQ-COMMON_LIBRARIES} )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
