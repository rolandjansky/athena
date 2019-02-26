################################################################################
# Package: RingerSelectorTools
################################################################################

# Declare package version:
set( RingerSelectorTools_version "\"RingerSelectorTools-00-00-18\"" )

# Declare the package name:
atlas_subdir( RingerSelectorTools )
# Extra dependencies based on the environment:
set( extra_pub_dep )
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
else()
  set( extra_pub_dep Control/AthenaKernel )
  set( extra_dep Control/AthContainers Tracking/TrkEvent/TrkTrackSummary GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/CxxUtils
                          Event/xAOD/xAODCaloRings
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODEventInfo
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
                          ${extra_pub_dep}
                          PRIVATE
                          Event/xAOD/xAODBase
                          Tools/PathResolver
                          ${extra_dep} )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Gpad Tree MathCore Hist RIO pthread )

atlas_add_library( RingerSelectorToolsEnumsLib
                   Root/enums/*.cxx
                   PUBLIC_HEADERS RingerSelectorTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( RingerSelectorToolsLib
                   Root/*.cxx
                   Root/procedures/*.cxx
                   Root/tools/*.cxx
                   PUBLIC_HEADERS RingerSelectorTools
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools CxxUtils xAODCaloRings xAODEgamma xAODTracking PATCoreLib EgammaAnalysisInterfacesLib RingerSelectorToolsEnumsLib
                   PRIVATE_LINK_LIBRARIES xAODBase PathResolver )
if( NOT XAOD_STANDALONE )
  atlas_add_component( RingerSelectorTools
                       src/components/*.cxx
                       INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                       LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel AthContainers GaudiKernel TrkTrackSummary RingerSelectorToolsLib )
endif()


atlas_add_dictionary( RingerSelectorToolsDict
                      RingerSelectorTools/RingerSelectorToolsDict.h
                      RingerSelectorTools/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} RingerSelectorToolsLib )

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
   LINK_LIBRARIES RingerSelectorToolsLib  ${ROOT_LIBRARIES} )
endif()

atlas_install_python_modules( python/*.py )

if( XAOD_STANDALONE )
message( STATUS "Using tag ${RingerSelectorTools_version} for all written RingerSelectorTools data." )
endif()
remove_definitions( -DRINGER_SELECTOR_PACKAGE_VERSION )
add_definitions( -DRINGER_SELECTOR_PACKAGE_VERSION=${RingerSelectorTools_version} )

