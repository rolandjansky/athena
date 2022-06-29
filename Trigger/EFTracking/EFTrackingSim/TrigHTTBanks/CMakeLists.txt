# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHTTBanks )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Create a library for non-Gaudi components
atlas_add_library( TrigHTTBanksLib
   src/*.cxx
   PUBLIC_HEADERS TrigHTTBanks
   INCLUDE_DIRS   ${EIGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrigHTTMapsLib TrigHTTObjectsLib
   PRIVATE_LINK_LIBRARIES AsgMessagingLib )

# Declare Gaudi component(s) in the package
atlas_add_component( TrigHTTBanks
   src/components/*.cxx
   LINK_LIBRARIES TrigHTTBanksLib )

# Install files from the package and run flake8 test:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )  


#atlas_add_test(             HTTFitConstantBank_test
#    SOURCES                 test/HTTFitConstantBank_test.cxx
#    LINK_LIBRARIES          TrigHTTBanksLib TrigHTTObjectsLib TrigHTTMapsLib
#)
