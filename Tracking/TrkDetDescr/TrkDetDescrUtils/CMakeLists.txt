################################################################################
# Package: TrkDetDescrUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkDetDescrUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetDescrUtils
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} SGTools GeoPrimitives GaudiKernel TrkEventPrimitives )

