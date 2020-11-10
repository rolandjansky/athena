# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_LocRecCorr )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( ALFA_LocRecCorr
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib AthenaPoolUtilities EventInfo ALFA_Geometry ALFA_RawEv ALFA_LocRecCorrEv ALFA_LocRecEv GaudiKernel GeneratorObjects AthenaKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )
