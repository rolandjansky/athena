################################################################################
# Package: FsrUtils
################################################################################

# Declare the package name:
atlas_subdir( FsrUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCore
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/IsolationSelection )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( FsrUtils
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODMuon PATInterfaces AthenaBaseComps xAODCore GaudiKernel IsolationSelectionLib )

# Install files from the package:
atlas_install_headers( FsrUtils )
atlas_install_joboptions( share/*.py )

