# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthExJobOptions )

# Component(s) in the package:
atlas_add_component( AthExJobOptions
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests EventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests in this package:
foreach( test BasicJobOptions;CustomToolJobOptions;CustomTopAlgorithmJobOptions )
   atlas_add_test( ${test}
      SCRIPT athena.py AthExJobOptions/AthExJobOptions_${test}.py
      LOG_IGNORE_PATTERN "AutoRetrieveTools"
      PROPERTIES TIMEOUT 600 )
endforeach()
