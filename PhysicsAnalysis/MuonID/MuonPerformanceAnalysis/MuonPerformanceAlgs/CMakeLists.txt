################################################################################
# Package: MuonPerformanceAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonPerformanceAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Reconstruction/RecoTools/RecoToolInterfaces
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceHistUtils
                          PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonResonanceTools
                          PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonTPTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonPerformanceAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} RecoToolInterfaces AthenaBaseComps xAODEventInfo xAODMuon GaudiKernel MuonPerformanceHistUtils MuonResonanceToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

