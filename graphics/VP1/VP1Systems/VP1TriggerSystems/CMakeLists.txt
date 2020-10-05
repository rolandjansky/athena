# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1TriggerSystems )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )
find_package( Qt5 COMPONENTS Core Gui  )



# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )
set( CMAKE_AUTOMOC_MOC_OPTIONS "-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED") 
# ### NOTE! ###
# "-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED" is needed when using Qt < 5.0 together with BOOST > 1.52. 
# Details: 
# - https://its.cern.ch/jira/browse/ATLASRECTS-3097
# - https://its.cern.ch/jira/browse/ATLASVPONE-184


# Generate resource files automatically:
set( CMAKE_AUTORCC TRUE )
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# Component(s) in the package:
atlas_add_library( VP1TriggerSystems VP1TriggerSystems/*.h src/*.h src/*.cxx src/*.qrc
                   PUBLIC_HEADERS VP1TriggerSystems
                   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AnalysisTriggerEvent TrigMuonEvent TrigSteeringEvent VP1Base GL StoreGateLib SGtests TrigDecisionToolLib TrigObjectMatchingLib
                   Qt5::Gui Qt5::Core
                   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel TrkParameters TrkTrack 
                 )

