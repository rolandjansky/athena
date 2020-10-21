# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkRIO_OnTrackCreator )

# Component(s) in the package:
atlas_add_component( TrkRIO_OnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
		             LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrkParameters TrkToolInterfaces CxxUtils AthenaPoolUtilities AtlasDetDescr Identifier InDetRIO_OnTrack MuonRIO_OnTrack TrkPrepRawData TrkRIO_OnTrack )

# Install files from the package:
atlas_install_joboptions( share/*.py )
