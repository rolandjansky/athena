# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecBackgroundAlgs )

# Component(s) in the package:
atlas_add_component( RecBackgroundAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthenaBaseComps xAODCaloEvent xAODEventInfo xAODJet GaudiKernel MuonIdHelpersLib MuonSegment MuonRecHelperToolsLib RecBackgroundEvent TrkSegment CaloGeoHelpers AthenaKernel GeoPrimitives LUCID_RawEvent LArRecEvent MuonPrepRawData TrkSpacePoint TileEvent BCM_CollisionTimeLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
