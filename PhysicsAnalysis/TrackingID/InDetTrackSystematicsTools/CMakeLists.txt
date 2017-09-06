# $Id: CMakeLists.txt 777904 2016-10-11 18:31:19Z krasznaa $

# Declare the name of the package:
atlas_subdir( InDetTrackSystematicsTools )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   Event/xAOD/xAODJet
   PRIVATE
   Control/CxxUtils
   Event/FourMomUtils
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODCore
   Tools/PathResolver
   ${extra_deps} )

# External(s):
find_package( ROOT COMPONENTS Core Hist RIO MathCore )

# Libraries in the package:
atlas_add_library( InDetTrackSystematicsToolsLib
   InDetTrackSystematicsTools/*.h Root/*.cxx
   PUBLIC_HEADERS InDetTrackSystematicsTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces xAODTracking
   xAODTruth xAODJet
   PRIVATE_LINK_LIBRARIES CxxUtils FourMomUtils xAODEventInfo PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( InDetTrackSystematicsTools
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AsgTools xAODCore
      xAODTracking GaudiKernel InDetTrackSystematicsToolsLib )
endif()

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( InDetTrackSystsToolTester
      util/InDetTrackSystsToolTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODTracking
      xAODCore CxxUtils InDetTrackSystematicsToolsLib )
endif()

atlas_add_executable( graph_to_function
   util/graph_to_function.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# This will allow us to put files into data/ to test new calibration files
# Uncomment this line after putting test files into data/, then run "make" again to install.
# atlas_install_data( data/*.root )
