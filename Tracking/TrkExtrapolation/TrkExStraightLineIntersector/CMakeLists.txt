################################################################################
# Package: TrkExStraightLineIntersector
################################################################################

# Declare the package name:
atlas_subdir( TrkExStraightLineIntersector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          GaudiKernel
                          AtlasTest/TestTools
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkExStraightLineIntersector
                     src/StraightLineIntersector.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps TrkExInterfaces TrkExUtils GaudiKernel TrkSurfaces TrkParameters )

# Install files from the package:
atlas_install_headers( TrkExStraightLineIntersector )

atlas_add_test( StraightLineIntersector_test
                SOURCES test/StraightLineIntersector_test.cxx
                LINK_LIBRARIES TrkExUtils TestTools GaudiKernel )
