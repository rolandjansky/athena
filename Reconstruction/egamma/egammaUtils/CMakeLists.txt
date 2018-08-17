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
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters 
			  PhysicsAnalysis/AnalysisCommon/AnalysisUtils)

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( egammaUtils
                   Root/*.cxx
                   PUBLIC_HEADERS egammaUtils
                   INCLUDE_DIRS 
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES EventPrimitives xAODCaloEvent xAODTracking TrkNeutralParameters 
		   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps FourMomUtils TrkSurfaces TrkEventPrimitives TrkParameters  AnalysisUtilsLib)

