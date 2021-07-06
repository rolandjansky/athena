# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ReadoutGeometryBase )

# External dependencies:
find_package( CLHEP )
find_package( Eigen ) # Needed by GeoModelKernel
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( ReadoutGeometryBase
   ReadoutGeometryBase/*.h ReadoutGeometryBase/*.icc src/*.cxx
   PUBLIC_HEADERS ReadoutGeometryBase
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                ${GEOMODEL_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES}
                  CxxUtils AthenaKernel AtlasDetDescr GeoPrimitives Identifier
                  GaudiKernel InDetCondTools TrkDetElementBase
                  TrkEventPrimitives
   PRIVATE_LINK_LIBRARIES AthenaBaseComps GeoModelUtilities TrkSurfaces )
