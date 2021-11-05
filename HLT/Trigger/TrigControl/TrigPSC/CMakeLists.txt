# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigPSC )

# External dependencies:
find_package( Boost )
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS Core )
find_package( tdaq-common COMPONENTS ers eformat eformat_write hltinterface )

# Component(s) in the package:
atlas_add_library( TrigPSC
                   src/*.cxx
                   PUBLIC_HEADERS TrigPSC
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Python_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils TrigKernel PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

