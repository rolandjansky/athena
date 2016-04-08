################################################################################
# Package: TrkDetDescrGeoModelCnv
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrGeoModelCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkDetDescr/TrkGeometry
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelKernel
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkDetDescrGeoModelCnv
                   src/*.cxx
                   PUBLIC_HEADERS TrkDetDescrGeoModelCnv
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives TrkGeometry
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} GeoModelKernel TrkVolumes )

