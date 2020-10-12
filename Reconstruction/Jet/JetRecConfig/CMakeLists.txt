# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetRecConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/JetRecTestCfg.py test/*.sh )
atlas_install_joboptions( share/*.py )

atlas_add_test( JetRecCfgTest
   SCRIPT test_JetRecCfg_build.sh
   POST_EXEC_SCRIPT nopost.sh )
