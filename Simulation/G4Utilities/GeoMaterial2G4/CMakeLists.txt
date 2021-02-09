# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: GeoMaterial2G4
################################################################################

# Declare the package name:
atlas_subdir( GeoMaterial2G4 )

# External dependencies:
find_package( Geant4 )
find_package( CLHEP )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( GeoMaterial2G4
                   src/*.cxx
                   PUBLIC_HEADERS GeoMaterial2G4
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps GeoModelUtilities )
