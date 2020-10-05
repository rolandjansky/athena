# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( BeamSpotConditions )

# Component(s) in the package.
atlas_add_component( BeamSpotConditions
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolUtilities AthenaBaseComps AthenaKernel
   StoreGateLib BeamSpotConditionsData )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
