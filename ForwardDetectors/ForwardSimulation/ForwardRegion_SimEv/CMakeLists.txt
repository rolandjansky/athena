# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardRegion_SimEv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( ForwardRegion_SimEv
                   src/*.cxx
                   PUBLIC_HEADERS ForwardRegion_SimEv
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel HitManagement )

atlas_add_dictionary( ForwardRegion_SimEvDict
                      ForwardRegion_SimEv/SimulationHitDict.h
                      ForwardRegion_SimEv/selection.xml
                      LINK_LIBRARIES ForwardRegion_SimEv )
