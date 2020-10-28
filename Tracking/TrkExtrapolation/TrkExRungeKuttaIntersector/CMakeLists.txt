# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExRungeKuttaIntersector )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO )

# Component(s) in the package:
atlas_add_component( TrkExRungeKuttaIntersector
                     src/IntersectorWrapper.cxx
                     src/RungeKuttaIntersector.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldConditions StoreGateLib TrkExInterfaces TrkExUtils TrkDetDescrUtils TrkGeometry TrkSurfaces TrkParameters TrkExInterfaces )

# Tests in the package:
atlas_add_test( RungeKuttaIntersector_test
                SOURCES test/RungeKuttaIntersector_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel MagFieldConditions PathResolver SGTools StoreGateLib TestTools TrkExInterfaces
                PROPERTIES TIMEOUT 300
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

