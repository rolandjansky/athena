# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthCreatorTools )

# Component(s) in the package:
atlas_add_component( TrkTruthCreatorTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps AthLinks AthContainers AtlasDetDescr GaudiKernel TrkEventUtils TrkTruthData TrkToolInterfaces GeneratorObjects TrkCompetingRIOsOnTrack TrkMeasurementBase TrkPrepRawData TrkRIO_OnTrack TrkTrack )
