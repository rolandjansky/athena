# $Id: CMakeLists.txt 767045 2016-08-09 11:46:34Z emoyse $
################################################################################
# Package: IsolationSelection
################################################################################

# Declare the package name:
atlas_subdir( IsolationSelection )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthContainers
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODBase
  Event/xAOD/xAODEgamma
  Event/xAOD/xAODMuon
  Event/xAOD/xAODPrimitives
  PhysicsAnalysis/AnalysisCommon/PATCore
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  InnerDetector/InDetRecTools/InDetTrackSelectionTool
  PRIVATE
  Control/AthenaBaseComps
  GaudiKernel
  Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_library( IsolationSelectionLib
  IsolationSelection/*.h Root/*.cxx
  PUBLIC_HEADERS IsolationSelection
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES AthContainers AsgTools xAODBase xAODEgamma xAODMuon
  xAODPrimitives PATCoreLib PATInterfaces InDetTrackSelectionToolLib
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )


atlas_add_component( IsolationSelection
  src/*.h src/*.cxx src/components/*.cxx
  LINK_LIBRARIES AsgTools AthenaBaseComps xAODEgamma xAODMuon GaudiKernel
  IsolationSelectionLib )
