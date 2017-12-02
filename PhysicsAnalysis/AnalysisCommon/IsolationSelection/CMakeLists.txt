################################################################################
# Package: IsolationSelection
################################################################################

# Declare the package name:
atlas_subdir( IsolationSelection )

# Extra dependencies based on the environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
elseif( XAOD_ANALYSIS )
   set( extra_dep Control/xAODRootAccess GaudiKernel )
else()
   set( extra_dep GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthContainers
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODBase
  Event/xAOD/xAODEgamma
  Event/xAOD/xAODMuon
  Event/xAOD/xAODEventInfo  
  Event/xAOD/xAODPrimitives
  PhysicsAnalysis/AnalysisCommon/PATCore
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  InnerDetector/InDetRecTools/TrackVertexAssociationTool
  InnerDetector/InDetRecTools/InDetTrackSelectionTool
  PRIVATE
  Control/AthenaBaseComps
  Tools/PathResolver
  ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist Tree )

set(extra_libs)
if ( NOT XAOD_ANALYSIS )
  set(extra_libs TrkExUtils)
endif()

# Libraries in the package:
atlas_add_library( IsolationSelectionLib
  IsolationSelection/*.h Root/*.cxx
  PUBLIC_HEADERS IsolationSelection
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES AthContainers AsgTools xAODBase xAODEgamma xAODMuon xAODEventInfo ${extra_libs}
  xAODPrimitives PATCoreLib PATInterfaces InDetTrackSelectionToolLib
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver FourMomUtils )

if( NOT XAOD_STANDALONE )
   atlas_add_component( IsolationSelection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgTools AthenaBaseComps GaudiKernel
      IsolationSelectionLib )
endif()

atlas_add_dictionary( IsolationSelectionDict
   IsolationSelection/IsolationSelectionDict.h
   IsolationSelection/selection.xml
   LINK_LIBRARIES IsolationSelectionLib )

# Executable(s) in the package:
if( XAOD_ANALYSIS )
   atlas_add_executable( testIsolationCloseByCorrectionTool
      util/testIsolationCloseByCorrectionTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODMuon xAODCore IsolationSelectionLib )

   atlas_add_executable( testIsolationSelectionTool
      util/testIsolationSelectionTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEgamma xAODMuon
      xAODPrimitives IsolationSelectionLib )
endif()

# Test(s) in the package:
if( XAOD_ANALYSIS )
   atlas_add_test( ut_reflex SCRIPT test/ut_reflex.py )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
