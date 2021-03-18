# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare package version:
set( RingerSelectorTools_version "\"RingerSelectorTools-00-00-18\"" )

# Declare the package name:
atlas_subdir( RingerSelectorTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Gpad Tree MathCore Hist RIO pthread )
find_package( onnxruntime )

atlas_add_library( RingerSelectorToolsEnumsLib
                   Root/enums/*.cxx
                   PUBLIC_HEADERS RingerSelectorTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} PATCoreAcceptLib )


atlas_add_library( RingerSelectorToolsLib
                   Root/*.cxx
                   Root/procedures/*.cxx
                   Root/tools/*.cxx
                   src/onnx/*.cxx
                   PUBLIC_HEADERS RingerSelectorTools
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${ONNXRUNTIME_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${ONNXRUNTIME_LIBRARIES} AsgTools CxxUtils xAODCaloRings xAODEgamma xAODTracking 
                   PATCoreLib EgammaAnalysisInterfacesLib RingerSelectorToolsEnumsLib xAODTrigRinger xAODTrigCalo xAODTrigEgamma
                   PRIVATE_LINK_LIBRARIES xAODBase PathResolver TrkTrackSummary AthOnnxruntimeServiceLib AthenaMonitoringKernelLib)

if( NOT XAOD_STANDALONE )
  atlas_add_component( RingerSelectorTools
                       src/components/*.cxx
                       INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                       LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel AthContainers GaudiKernel TrkTrackSummary RingerSelectorToolsLib )
endif()


atlas_add_dictionary( RingerSelectorToolsDict
                      RingerSelectorTools/RingerSelectorToolsDict.h
                      RingerSelectorTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} RingerSelectorToolsLib )

atlas_add_dictionary( RingerSelectorToolsEnumsDict
                      RingerSelectorTools/RingerSelectorToolsEnumsDict.h
                      RingerSelectorTools/selection_enums.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} RingerSelectorToolsEnumsLib )

# Install files from the package:
if( XAOD_STANDALONE )
atlas_add_executable( dumpRings
   util/dumpRings.cxx
   LINK_LIBRARIES RingerSelectorToolsLib ${ROOT_LIBRARIES} )

atlas_add_executable( ringsHist
   util/ringsHist.cxx
   LINK_LIBRARIES RingerSelectorToolsLib ${ROOT_LIBRARIES} )
endif()

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

if( XAOD_STANDALONE )
message( STATUS "Using tag ${RingerSelectorTools_version} for all written RingerSelectorTools data." )
endif()
remove_definitions( -DRINGER_SELECTOR_PACKAGE_VERSION )
add_definitions( -DRINGER_SELECTOR_PACKAGE_VERSION=${RingerSelectorTools_version} )

