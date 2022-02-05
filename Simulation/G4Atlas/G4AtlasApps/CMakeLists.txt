# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( G4AtlasApps )

# Tests in the package:
atlas_add_test(test_AtlasG4_tf_configuration
               SCRIPT test/test_AtlasG4_tf_configuration.py
               POST_EXEC_SCRIPT nopost.sh
               PROPERTIES RESOURCE_LOCK AtlasG4_Tf )

if( NOT SIMULATIONBASE )
   atlas_add_test(test_AtlasG4_cosmics_configuration
                  SCRIPT test/test_AtlasG4_cosmics_configuration.py
                  POST_EXEC_SCRIPT nopost.sh
                  PROPERTIES RESOURCE_LOCK AtlasG4_Tf )

   atlas_add_test(test_TestBeam_tf_configuration
                  SCRIPT test/test_TestBeam_tf_configuration.py
                  POST_EXEC_SCRIPT nopost.sh
                  PROPERTIES RESOURCE_LOCK AtlasG4_Tf )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

