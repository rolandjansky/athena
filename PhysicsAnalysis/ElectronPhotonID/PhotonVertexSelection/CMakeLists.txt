################################################################################
# Package: PhotonVertexSelection
################################################################################

# Declare the package name:
atlas_subdir( PhotonVertexSelection )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTracking
   PRIVATE
   Control/AthContainers
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO Physics TMVA )

# Libraries in the package:
atlas_add_library( PhotonVertexSelectionLib
   PhotonVertexSelection/*.h Root/*.cxx
   PUBLIC_HEADERS PhotonVertexSelection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo
   xAODTracking
   PRIVATE_LINK_LIBRARIES IsolationCorrectionsLib PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PhotonVertexSelection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODEgamma xAODTracking AthContainers AthenaBaseComps
      GaudiKernel PhotonVertexSelectionLib )
endif()

atlas_add_dictionary( PhotonVertexSelectionDict
   PhotonVertexSelection/PhotonVertexSelectionDict.h
   PhotonVertexSelection/selection.xml
   LINK_LIBRARIES PhotonVertexSelectionLib )

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( PhotonVertexSelection_test
      SCRIPT test/testPhotonVertexSelection.py )
endif()
