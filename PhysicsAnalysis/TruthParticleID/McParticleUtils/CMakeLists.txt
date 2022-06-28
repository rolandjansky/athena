# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( McParticleUtils )

# External dependencies:
find_package( Boost )
find_package( HepPDT )
find_package( Python COMPONENTS Development )

# Component(s) in the package:
atlas_add_library( McParticleUtils
                   src/*.cxx
                   PUBLIC_HEADERS McParticleUtils
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${Python_INCLUDE_DIRS}
                   LINK_LIBRARIES AnalysisUtilsLib AthContainers AthenaBaseComps AtlasHepMCLib EventKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${Python_LIBRARIES} AthenaKernel GaudiKernel RootUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

