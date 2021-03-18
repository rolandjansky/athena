# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigFastTrackFinder )

# Component(s) in the package:
atlas_add_component( TrigFastTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib BeamSpotConditionsData CxxUtils PathResolver InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces SiSPSeededTrackFinderData TrigInDetEvent TrigInDetPattRecoEvent TrigInDetPattRecoTools TrigInDetToolInterfacesLib TrigSteeringEvent TrkEventPrimitives TrkEventUtils TrkParameters TrkRIO_OnTrack TrkToolInterfaces TrkTrack TrigAccelEvent TrigInDetAccelerationToolLib TrigInDetAccelerationServiceLib TrigT1Interfaces xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/*.lut )
