################################################################################
# Package: TrigCommon
################################################################################

# Declare the package name:
atlas_subdir( TrigCommon )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( bin/athenaHLT.py )

# Aliases:
atlas_add_alias( athenaHLT "athenaHLT.py" )

# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=F,E7,E9,W6 --ignore=E701 ${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_SOURCE_DIR}/bin
   POST_EXEC_SCRIPT nopost.sh )

# Tests:
atlas_add_test( AthHLT
   SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/AthHLT.py )
