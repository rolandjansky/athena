# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTagCalibration )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library( JetTagCalibrationLib
                   src/*.cxx
                   PUBLIC_HEADERS JetTagCalibration
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${LWTNN_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel MVAUtils StoreGateLib
                   PRIVATE_LINK_LIBRARIES FileCatalog PoolSvcLib )

atlas_add_component( JetTagCalibration
                     src/components/*.cxx
                     LINK_LIBRARIES JetTagCalibrationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
