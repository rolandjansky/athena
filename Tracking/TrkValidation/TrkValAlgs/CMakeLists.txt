# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkValAlgs )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( TrkValAlgs
   TrkValAlgs/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps
   GaudiKernel TrkParameters TrkTrack TrkTruthData AtlasDetDescr
   GeneratorObjects TrkSurfaces TrkEventUtils TrkMaterialOnTrack
   TrkParticleBase TrkPrepRawData TrkRIO_OnTrack TrkSegment
   TrkTrackSummary VxVertex TrkToolInterfaces TrkValEvent TrkValInterfaces )
