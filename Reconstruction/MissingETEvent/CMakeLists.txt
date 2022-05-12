# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MissingETEvent )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( MissingETEvent
                   src/*.cxx
                   PUBLIC_HEADERS MissingETEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel AtlasHepMCLib CaloEvent CaloGeoHelpers AthContainers Navigation NavFourMom
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CxxUtils TruthHelper )

atlas_add_dictionary( MissingETEventDict
                      MissingETEvent/MissingETEventDict.h
                      MissingETEvent/selection.xml
                      LINK_LIBRARIES MissingETEvent )
