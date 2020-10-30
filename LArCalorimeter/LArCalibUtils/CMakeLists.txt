# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCalibUtils )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )
find_package( Eigen )
find_package( GSL )
find_package( ROOT COMPONENTS Minuit Core Tree MathCore Hist RIO Graf )
find_package( TBB )

# Component(s) in the package:
atlas_add_component( LArCalibUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthContainers AthenaBaseComps AthenaKernel StoreGateLib Identifier GaudiKernel LArElecCalib LArIdentifier LArRawConditions LArRawEvent LArRawUtilsLib LArRecConditions LArRecUtilsLib LArCablingLib TBEvent CaloDetDescrLib AthenaPoolUtilities xAODEventInfo LArBadChannelToolLib LArCOOLConditions LArSimEvent TrigAnalysisInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
