# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkMaterialProvider )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkMaterialProvider
                     src/TrkMaterialProviderTool.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps TrkDetDescrInterfaces TrkGeometry TrkSurfaces TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces AtlasDetDescr Identifier EventPrimitives GaudiKernel MuidEvent MuidInterfaces muonEvent RecoToolInterfaces TrkEventUtils TrkMaterialOnTrack MagFieldConditions )

# Install files from the package:
atlas_install_headers( TrkMaterialProvider )

