# $Id: CMakeLists.txt 797759 2017-02-18 01:24:37Z zmarshal $
################################################################################
# Package: DerivationFrameworkAnalysisTests
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkAnalysisTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PRIVATE
   Control/xAODRootAccess
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODTruth
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree Hist )

# Executable(s) in the package:
atlas_add_executable( TruthDerivationTester
   util/TruthDerivationTester.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess AsgTools xAODEventInfo
   xAODTruth xAODJet xAODMissingET )

