# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiSPSeededTrackFinder )

# Component(s) in the package:
atlas_add_component( SiSPSeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib GaudiKernel BeamSpotConditionsData InDetRecToolInterfaces IRegionSelector RoiDescriptor TrkCaloClusterROI TrkGeometry TrkSurfaces TrkSpacePoint TrkTrack TrkExInterfaces xAODEventInfo SiSPSeededTrackFinderData TrkPatternParameters TrkRIO_OnTrack TrkEventUtils TrkToolInterfaces)

# Run tests:
atlas_add_test( SiSPSeededTracksStandalone
                SCRIPT athena.py --threads=5 SiSPSeededTrackFinder/SiSPSeededTracksStandalone.py
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )

atlas_add_test( SiSPSeededTracksStandaloneFromESD
                SCRIPT athena.py --threads=1 SiSPSeededTrackFinder/SiSPSeededTracksStandaloneFromESD.py
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
