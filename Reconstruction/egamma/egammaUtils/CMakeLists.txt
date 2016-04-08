################################################################################
# Package: egammaUtils
################################################################################

# Declare the package name:
atlas_subdir( egammaUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventPrimitives
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking
                          Tracking/TrkEvent/TrkNeutralParameters
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/FourMomUtils
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( egammaUtils
                   Root/*.cxx
                   PUBLIC_HEADERS egammaUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives xAODCaloEvent xAODTracking TrkNeutralParameters AnalysisUtilsLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps FourMomUtils TrkSurfaces TrkEventPrimitives TrkParameters )

