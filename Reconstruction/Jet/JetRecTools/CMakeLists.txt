# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( JetRecTools )

# External dependencies.
find_package( FastJet )
find_package( FastJetContrib COMPONENTS SoftKiller ConstituentSubtractor )
find_package( ROOT COMPONENTS Core MathCore )

set( extra_libs )
set( extra_libs_pub )
if( NOT XAOD_ANALYSIS AND NOT XAOD_STANDALONE)
  set( extra_libs AthenaMonitoringKernelLib )
  set( extra_libs_pub InDetRecToolInterfaces )
endif()


# Component(s) in the package.
atlas_add_library( JetRecToolsLib
   JetRecTools/*.h Root/*.cxx src/*.cxx
   PUBLIC_HEADERS JetRecTools
   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${FASTJET_LIBRARIES} AsgMessagingLib AthContainers AsgTools xAODPFlow
   xAODTracking xAODCore xAODBase xAODCaloEvent xAODEgamma xAODJet
   TrackVertexAssociationToolLib JetEDM JetInterface JetRecLib PathResolver
   InDetTrackSelectionToolLib PFlowUtilsLib ${extra_libs_pub}
   PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} ${ROOT_LIBRARIES} ${extra_libs} xAODTruth )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetRecTools
      src/components/*.cxx
      LINK_LIBRARIES JetRecToolsLib ${extra_libs_pub})
endif()


if( XAOD_STANDALONE )
atlas_add_dictionary( JetRecToolsDict
                      JetRecTools/JetRecToolsDict.h
                      JetRecTools/selection.xml
                      LINK_LIBRARIES JetRecToolsLib  )
endif()

# Install files from the package.
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
