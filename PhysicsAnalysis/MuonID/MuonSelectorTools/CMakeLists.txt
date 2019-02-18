# $Id: CMakeLists.txt 777718 2016-10-11 16:52:13Z krasznaa $
################################################################################
# Package: MuonSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSelectorTools )

# Extra dependencies, based on the environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODMuon
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
   PRIVATE
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTracking
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO TMVA )

# Libraries in the package:
atlas_add_library( MuonSelectorToolsLib
   MuonSelectorTools/*.h Root/*.cxx
   PUBLIC_HEADERS MuonSelectorTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODMuon PATCoreLib MuonAnalysisInterfacesLib
   PRIVATE_LINK_LIBRARIES xAODTracking PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( MuonSelectorTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps GaudiKernel xAODCore xAODEventInfo xAODMuon MuonAnalysisInterfacesLib
      MuonSelectorToolsLib )
endif()

atlas_add_dictionary( MuonSelectorToolsDict
   MuonSelectorTools/MuonSelectorToolsDict.h
   MuonSelectorTools/selection.xml
   LINK_LIBRARIES MuonSelectorToolsLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( MuonSelectorToolsTester
      util/MuonSelectorToolsTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODMuon
      xAODTracking xAODCore MuonSelectorToolsLib )
endif()


# Test(s) in the package: 
if( XAOD_STANDALONE )
   atlas_add_test( ut_MuonSelectorToolsTester_data
      SOURCES test/ut_MuonSelectorToolsTester_data.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
