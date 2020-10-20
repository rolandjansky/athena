# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExSolenoidalIntersector )

# External dependencies:
find_package( ROOT )

atlas_add_library( TrkExSolenoidalIntersectorLib
                   src/SolenoidalIntersector.cxx
                   src/SolenoidParametrization.cxx
                   src/SolenoidParametrizationCondAlg.cxx
                   PUBLIC_HEADERS TrkExSolenoidalIntersector
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel CxxUtils GeoPrimitives GaudiKernel MagFieldConditions StoreGateLib TrkExInterfaces TrkExUtils
                   PRIVATE_LINK_LIBRARIES EventPrimitives MagFieldElements TrkParameters TrkSurfaces )
# Component(s) in the package:
atlas_add_component( TrkExSolenoidalIntersector
                     src/components/*.cxx
                     LINK_LIBRARIES TrkExSolenoidalIntersectorLib )

# Tests in the package:
atlas_add_test( SolenoidParametrization_test
                SOURCES test/SolenoidParametrization_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel GaudiKernel MagFieldConditions MagFieldElements PathResolver TestTools TrkExSolenoidalIntersectorLib
                PROPERTIES TIMEOUT 300
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( SolenoidalIntersector_test
                SOURCES test/SolenoidalIntersector_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel CxxUtils GaudiKernel MagFieldConditions MagFieldElements PathResolver SGTools StoreGateLib TestTools TrkExInterfaces TrkExSolenoidalIntersectorLib
                PROPERTIES TIMEOUT 300
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

