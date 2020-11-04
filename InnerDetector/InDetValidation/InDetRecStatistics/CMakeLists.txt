# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetRecStatistics )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( InDetRecStatistics
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps Identifier GaudiKernel GeneratorObjects TrkTrack TrkTrackSummary AtlasDetDescr IdDictDetDescr TruthHelper InDetIdentifier InDetRIO_OnTrack TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkRIO_OnTrack TrkTruthData VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.py )
