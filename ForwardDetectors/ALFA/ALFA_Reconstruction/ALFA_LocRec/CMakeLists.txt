# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_LocRec )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist )

# Component(s) in the package:
atlas_add_library( ALFA_LocRecLib
                   ALFA_LocRec/*.h src/*.cxx
                   INTERFACE
                   PUBLIC_HEADERS ALFA_LocRec
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ALFA_Geometry ALFA_LocRecEv ALFA_RawEv AthenaBaseComps AthenaKernel AthenaPoolUtilities AtlasHepMCLib EventInfo GaudiKernel GeneratorObjects StoreGateLib )

atlas_add_component( ALFA_LocRec
                     src/components/*.cxx
                     LINK_LIBRARIES ALFA_LocRecLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
