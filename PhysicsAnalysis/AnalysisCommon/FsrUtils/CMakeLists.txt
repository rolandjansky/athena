################################################################################
# Package: FsrUtils
################################################################################

# Declare the package name:
atlas_subdir( FsrUtils )

# Extra dependencies based on the environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
else()
   set( extra_dep GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODMuon
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
   PRIVATE
   Control/AthenaBaseComps
   Event/xAOD/xAODCore
   ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Core Physics RIO )

# Libraries in the package:
atlas_add_library( FsrUtilsLib
   FsrUtils/*.h Root/*.cxx
   PUBLIC_HEADERS FsrUtils
   LINK_LIBRARIES AsgTools xAODMuon xAODEgamma PATInterfaces
   IsolationSelectionLib IsolationCorrectionsLib ElectronPhotonFourMomentumCorrectionLib)

if( NOT XAOD_STANDALONE )
   atlas_add_component( FsrUtils
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel xAODMuon
      FsrUtilsLib )
endif()

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( FsrPhotonToolTester
      util/FsrPhotonToolTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODMuon xAODCore 
      FsrUtilsLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
