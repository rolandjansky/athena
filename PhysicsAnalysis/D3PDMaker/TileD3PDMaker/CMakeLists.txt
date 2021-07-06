# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileD3PDMaker )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( TileD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaBaseComps AtlasHepMCLib CaloEvent CaloGeoHelpers D3PDMakerUtils EventInfo GaudiKernel GeneratorObjects GeoModelInterfaces GeoModelUtilities GeoPrimitives ITrackToVertex Identifier RDBAccessSvcLib RecoToolInterfaces StoreGateLib TileEvent TileIdentifier TrkParameters TrkParametersIdentificationHelpers VxVertex xAODCaloEvent xAODEventInfo xAODMissingET xAODMuon xAODPrimitives xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

