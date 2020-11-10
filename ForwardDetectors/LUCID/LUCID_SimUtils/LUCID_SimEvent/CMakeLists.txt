# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_SimEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LUCID_SimEvent
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_SimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel Identifier GeneratorObjects HitManagement )

atlas_add_dictionary( LUCID_SimEventDict
                      LUCID_SimEvent/LUCID_SimEventDict.h
                      LUCID_SimEvent/selection.xml
                      LINK_LIBRARIES LUCID_SimEvent )
