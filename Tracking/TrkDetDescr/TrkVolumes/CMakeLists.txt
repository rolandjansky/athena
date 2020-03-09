################################################################################
# Package: TrkVolumes
################################################################################

# Declare the package name:
atlas_subdir( TrkVolumes )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometrySurfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters 
			  Control/CxxUtils)

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkVolumes
                   src/*.cxx
                   PUBLIC_HEADERS TrkVolumes
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
		   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel TrkDetDescrUtils TrkGeometrySurfaces TrkSurfaces TrkEventPrimitives TrkParameters CxxUtils)

