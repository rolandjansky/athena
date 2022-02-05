# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: MuonGeoModel
################################################################################

# Declare the package name:
atlas_subdir( MuonGeoModel )

# External dependencies:
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( MuonGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonGeoModel
                   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects StoreGateLib MuonIdHelpersLib MuonAGDDDescription AGDDControl AGDDKernel AmdcDbLib CxxUtils
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities IdDictDetDescr MuonDetDescrUtils )

atlas_add_component( MuonGeoModel
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaKernel StoreGateLib GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects AthenaPoolUtilities AGDDKernel IdDictDetDescr MuonAGDDDescription MuonDetDescrUtils MuonIdHelpersLib MuonGeoModelLib AmdcDbLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
