# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloClusterCorrection )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CaloClusterCorrectionLib
                   src/*.cxx
                   PUBLIC_HEADERS CaloClusterCorrection
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel CaloConditions CaloEvent CaloInterfaceLib CaloRecLib CaloUtilsLib CxxUtils GaudiKernel StoreGateLib TileConditionsLib xAODCaloEvent
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps CaloDetDescrLib CaloGeoHelpers CaloIdentifier LArRecConditions PathResolver )

atlas_add_component( CaloClusterCorrection
                     src/components/*.cxx
                     LINK_LIBRARIES CaloClusterCorrectionLib )

# Test(s) in the package:
atlas_add_test( interpolate_test
                SOURCES
                test/interpolate_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} CaloClusterCorrectionLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

