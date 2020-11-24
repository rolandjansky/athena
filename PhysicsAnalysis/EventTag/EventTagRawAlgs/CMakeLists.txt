# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventTagRawAlgs )

# Component(s) in the package:
atlas_add_component( EventTagRawAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel TrkSegment CaloEvent CommissionEvent StoreGateLib xAODCaloEvent InDetBCM_RawData InDetPrepRawData LArRecEvent TagEvent TileEvent TileIdentifier TrkSpacePoint TrkTrack TrkTrackSummary MagFieldConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )

