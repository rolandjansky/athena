# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_ReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( HGTD_ReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS HGTD_ReadoutGeometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel SGTools AtlasDetDescr GeoPrimitives Identifier ReadoutGeometryBase GaudiKernel HGTD_Identifier TrkSurfaces StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities DetDescrConditions IdDictDetDescr )
