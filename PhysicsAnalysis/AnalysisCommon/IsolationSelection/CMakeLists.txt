# $Id: CMakeLists.txt 742362 2016-04-23 09:23:48Z krasznaa $
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
  xAODPrimitives PATCoreLib
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )

atlas_add_component( IsolationSelection
  src/*.h src/*.cxx src/components/*.cxx
  LINK_LIBRARIES AsgTools AthenaBaseComps xAODEgamma xAODMuon GaudiKernel
  IsolationSelectionLib )
