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
atlas_add_test( test_AthHLT
   SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/AthHLT.py
   POST_EXEC_SCRIPT nopost.sh
   PROPERTIES TIMEOUT 300 )

atlas_add_test( athenaHLT_jo_noopts
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/share/test_athenaHLT.sh dummy.py)

atlas_add_test( athenaHLT_jo_allopts
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/share/test_athenaHLT.sh --number-of-events 3 --skip-events 7 -l ERROR,FATAL -c 'x=1' -c 'y=2' -C 'x=2' -o out.data --threads 2 --nprocs 4 --timeout 777 --oh-monitoring --python-setup mysetup.py --partition mypart dummy.py)
