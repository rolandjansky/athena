# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( McParticleUtils )

# External dependencies:
find_package( Boost )
find_package( HepPDT )
find_package( Python COMPONENTS Development )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( McParticleUtils
                   src/*.cxx
                   PUBLIC_HEADERS McParticleUtils
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${Python_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AtlasHepMCLib AthenaKernel AthContainers EventKernel GaudiKernel AnalysisUtilsLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${Python_LIBRARIES} RootUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

