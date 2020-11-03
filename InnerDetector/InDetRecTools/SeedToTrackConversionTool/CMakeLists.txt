# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SeedToTrackConversionTool )

# Component(s) in the package:
atlas_add_component( SeedToTrackConversionTool
                     src/*.cxx
                     src/components/*.cxx
		             LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities GaudiKernel InDetPrepRawData InDetRecToolInterfaces SiSPSeededTrackFinderData StoreGateLib TrkExInterfaces TrkRIO_OnTrack TrkToolInterfaces TrkTrack )
