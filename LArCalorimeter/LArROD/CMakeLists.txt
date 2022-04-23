# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArROD )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library( LArRODLib
                   LArROD/*.h
                   INTERFACE
                   PUBLIC_HEADERS LArROD
                   LINK_LIBRARIES CaloIdentifier GaudiKernel Identifier LArRawEvent )

atlas_add_component( LArROD
                     src/*.cxx
                     src/tests/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     DEFINITIONS ${CLHEP_DEFINITIONS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${LWTNN_LIBRARIES} ${ROOT_LIBRARIES} AthAllocators AthContainers AthenaBaseComps AthenaKernel AthenaPoolUtilities CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CommissionEvent GaudiKernel GeoModelInterfaces Identifier LArCOOLConditions LArCablingLib LArDigitizationLib LArElecCalib LArIdentifier LArRODLib LArRawConditions LArRawEvent LArRawUtilsLib LArRecConditions LArRecUtilsLib LArSimEvent StoreGateLib TBEvent xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

atlas_add_test( LArRawChannelBuilderAlg 
		SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/LArRawChannelBuilderAlgConfig.py
		POST_EXEC_SCRIPT " /usr/bin/diff LArRawChannels.txt ${CMAKE_CURRENT_SOURCE_DIR}/share/LArRawChannels.txt.ref > diff.log " )
