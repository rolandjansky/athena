################################################################################
# Package: MuonHistUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonHistUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkValidation/TrkValHistUtils
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonHistUtils
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS MuonHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMuon xAODTracking xAODTrigMuon xAODTrigger xAODTruth TrkEventPrimitives TrkValHistUtils MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives )

