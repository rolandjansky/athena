# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigFake )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigFake
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GeneratorObjects TrigConfHLTData TrigInDetEvent TrigSteeringEvent TrigSteeringLib TrigT1Interfaces TrigT1Result )
