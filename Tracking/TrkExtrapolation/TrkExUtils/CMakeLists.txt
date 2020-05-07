################################################################################
# Package: TrkExUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkExUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Tracking/TrkEvent/TrkPatternParameters 
			  Control/CxxUtils
			  )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkExUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkExUtils
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives GaudiKernel TrkDetDescrUtils TrkGeometry 
		   TrkSurfaces TrkEventPrimitives TrkMaterialOnTrack TrkNeutralParameters TrkParameters CxxUtils
                   PRIVATE_LINK_LIBRARIES TrkPatternParameters )

