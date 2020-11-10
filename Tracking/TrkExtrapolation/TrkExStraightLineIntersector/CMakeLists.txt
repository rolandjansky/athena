# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExStraightLineIntersector )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkExStraightLineIntersector
                     src/StraightLineIntersector.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps TrkExInterfaces TrkExUtils TrkSurfaces TrkParameters TrkExInterfaces )

# Tests in the package:
atlas_add_test( StraightLineIntersector_test
                SOURCES test/StraightLineIntersector_test.cxx
                LINK_LIBRARIES TestTools GaudiKernel TrkExInterfaces )
