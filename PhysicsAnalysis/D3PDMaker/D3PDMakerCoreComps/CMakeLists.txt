# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( D3PDMakerCoreComps )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_component( D3PDMakerCoreComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel AthenaPoolCnvSvcLib D3PDMakerInterfaces D3PDMakerUtils GaudiKernel ParticleEvent RootUtils SGTools )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests in the package:
atlas_add_test( flagTestLOD_t
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/flagTestLOD_t.py
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( StackedDict_t
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/StackedDict_t.py
                POST_EXEC_SCRIPT nopost.sh )
