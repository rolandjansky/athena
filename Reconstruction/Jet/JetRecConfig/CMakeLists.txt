# $Id: CMakeLists.txt 801725 2017-03-28 19:23:28Z khoo $
################################################################################
# Package: JetRecConfig
################################################################################

# Declare the package name:
atlas_subdir( JetRecConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/JetRecTestCfg.py test/*.sh )
atlas_install_joboptions( share/*.py )

atlas_add_test( JetRecCfgTest
   SCRIPT test_JetRecCfg_build.sh
   POST_EXEC_SCRIPT nopost.sh )
