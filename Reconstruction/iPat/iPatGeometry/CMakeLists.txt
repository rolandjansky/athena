################################################################################
# Package: iPatGeometry
################################################################################

# Declare the package name:
atlas_subdir( iPatGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Event/EventPrimitives
                          GaudiKernel )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( iPatGeometry
                   src/DetectorIntersect.cxx
                   src/MaterialAllocator.cxx
                   src/PixelTrapezoid.cxx
                   src/SiliconDetector.cxx
                   src/SiliconDetectorBuilder.cxx
                   src/SiliconLayer.cxx
                   src/SiliconRectangle.cxx
                   src/SiliconRing.cxx
                   src/SiliconTrapezoid.cxx
                   PUBLIC_HEADERS iPatGeometry
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives Identifier InDetReadoutGeometry TrkSurfaces
                   PRIVATE_LINK_LIBRARIES EventPrimitives GaudiKernel )

