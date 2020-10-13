# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileTrackingGeometry )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( TileTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaBaseComps GaudiKernel TrkDetDescrInterfaces CaloDetDescrLib StoreGateLib TileDetDescr TrkDetDescrGeoModelCnv TrkDetDescrUtils TrkGeometry TrkSurfaces TrkVolumes CaloTrackingGeometryLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
