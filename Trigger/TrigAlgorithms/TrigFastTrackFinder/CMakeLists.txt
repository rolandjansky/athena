# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigFastTrackFinder )

# Component(s) in the package:
atlas_add_component( TrigFastTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib BeamSpotConditionsData CxxUtils GaudiKernel InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces SiSPSeededTrackFinderData SiSpacePointsSeed TrigInDetEvent TrigInDetPattRecoEvent TrigInDetPattRecoTools TrigInDetToolInterfacesLib TrigInterfacesLib TrigNavigationLib TrigSteeringEvent TrigTimeAlgsLib TrkEventPrimitives TrkEventUtils TrkParameters TrkRIO_OnTrack TrkToolInterfaces TrkTrack TrkTrackSummary TrigAccelEvent TrigInDetAccelerationToolLib TrigInDetAccelerationServiceLib)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
