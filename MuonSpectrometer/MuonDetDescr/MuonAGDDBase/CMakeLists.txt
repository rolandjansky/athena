# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: MuonAGDDBase
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDDBase )

find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_library( MuonAGDDBase
                   src/*.cxx
                   PUBLIC_HEADERS MuonAGDDBase
                   LINK_LIBRARIES AGDDControl AGDDKernel GaudiKernel MuonAGDDDescription StoreGateLib SGtests MuonGeoModelLib
                   PRIVATE_LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AGDDModel )

