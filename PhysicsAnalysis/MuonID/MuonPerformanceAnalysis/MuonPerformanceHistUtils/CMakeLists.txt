################################################################################
# Package: MuonPerformanceHistUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonPerformanceHistUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          MuonSpectrometer/MuonValidation/MuonHistogramming/MuonHistUtils
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
                          Tracking/TrkValidation/TrkValHistUtils
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonPerformanceHistUtils
                   Root/*.cxx
                   PUBLIC_HEADERS MuonPerformanceHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODMissingET xAODMuon xAODTruth GaudiKernel MuonHistUtils TrkValHistUtils MuonEfficiencyCorrectionsLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODEventInfo xAODTracking )

