# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: MuonTrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackingGeometry )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( MuonTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaBaseComps GaudiKernel TrkDetDescrGeoModelCnv TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry TrkVolumes StoreGateLib SGtests GeoPrimitives MuonReadoutGeometry MuonIdHelpersLib TrkGeometrySurfaces TrkSurfaces GeoModelUtilities SubDetectorEnvelopesLib )

# Install files from the package:
atlas_install_headers( MuonTrackingGeometry )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

