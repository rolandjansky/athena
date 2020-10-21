# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArROD )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArRODLib
                   src/*.cxx
                   src/tests/*.cxx
                   PUBLIC_HEADERS LArROD
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier AthContainers AthenaBaseComps AthenaPoolUtilities Identifier GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArRecConditions TBEvent StoreGateLib LArCablingLib LArElecCalib LArRawUtilsLib LArRecUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloDetDescrLib CommissionEvent LArCOOLConditions )

atlas_add_component( LArROD
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES LArRODLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( LArRawChannelBuilderAlg 
		SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/LArRawChannelBuilderAlgConfig.py
		POST_EXEC_SCRIPT " /usr/bin/diff LArRawChannels.txt ${CMAKE_CURRENT_SOURCE_DIR}/share/LArRawChannels.txt.ref > diff.log " )
