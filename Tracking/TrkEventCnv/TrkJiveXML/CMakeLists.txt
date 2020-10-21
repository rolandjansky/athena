# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkJiveXML )

# Component(s) in the package:
atlas_add_component( TrkJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkParameters JiveXMLLib AthLinks AthContainers StoreGateLib AtlasDetDescr Particle TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkMeasurementBase TrkParticleBase TrkRIO_OnTrack TrkSegment TrkTrack TrkTrackLink TrkTrackSummary TrkTruthData VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.py )
