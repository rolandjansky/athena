# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventBoost )

# External dependencies:
find_package( CLHEP )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( EventBoost
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel StoreGateLib SGtests GeneratorObjects )

# Install files from the package:
atlas_install_headers( EventBoost )

