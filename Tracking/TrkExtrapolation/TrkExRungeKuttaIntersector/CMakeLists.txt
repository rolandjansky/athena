################################################################################
# Package: TrkExRungeKuttaIntersector
################################################################################

# Declare the package name:
atlas_subdir( TrkExRungeKuttaIntersector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          AtlasTest/TestTools
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrkExRungeKuttaIntersector
                     src/IntersectorWrapper.cxx
                     src/RungeKuttaIntersector.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldInterfaces TrkExInterfaces TrkExUtils TrkDetDescrUtils TrkGeometry TrkSurfaces TrkParameters )

# Install files from the package:
atlas_install_headers( TrkExRungeKuttaIntersector )


atlas_add_test( RungeKuttaIntersector_test
                SOURCES test/RungeKuttaIntersector_test.cxx
                LINK_LIBRARIES TrkExUtils GaudiKernel TestTools
                EXTRA_PATTERNS "^AtlasFieldSvc +INFO"
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

