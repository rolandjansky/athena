# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RegionSelector )

# Component(s) in the package:
atlas_add_library( RegionSelectorLib
                   src/*.cxx
                   PUBLIC_HEADERS RegionSelector
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel GeoModelInterfaces IRegionSelector Identifier RegSelLUT
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities CxxUtils InDetConditionsSummaryService PathResolver StoreGateLib )

atlas_add_component( RegionSelector
                     src/components/*.cxx
                     LINK_LIBRARIES RegionSelectorLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests in the package:
atlas_add_test( RegSelConfigTest
                SCRIPT python -m RegionSelector.RegSelConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
