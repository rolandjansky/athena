# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZDC_SimuDigitization )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( ZDC_SimuDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib ZDC_SimEvent ZdcEvent GaudiKernel HitManagement Identifier )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

