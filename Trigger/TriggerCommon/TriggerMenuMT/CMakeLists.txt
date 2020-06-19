# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TriggerMenuMT )

# External dependencies:
find_package( six )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              python/LVL1MenuConfig
                              python/L1
                              python/HLTMenuConfig
		                      POST_BUILD_CMD ${ATLAS_FLAKE8})

atlas_install_scripts( scripts/generateMenuMT.py
                       scripts/generateLVL1MenuMT.py
                       scripts/generateL1TopoMenuMT.py
                       scripts/generateL1MenuRun3.py
                       scripts/verify_menu_config.py
                       POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_install_scripts( scripts/generateL1MenuMT.sh )
atlas_install_xmls( data/*.xml )

# Tests in the package:
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6,E101 --enable-extension=ATL900,ATL901,ATL902
                ${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( ViewCFTest
                SCRIPT python -m unittest -v TriggerMenuMT.HLTMenuConfig.Test.ViewCFTest
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( generateMenuMT_newJO
                SCRIPT python -m TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1_newJO
                PROPERTIES TIMEOUT 500 )

atlas_add_test( generateMenuMT
                SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_HLTmenu.sh
                PROPERTIES TIMEOUT 4000 )

# Test L1 trigger menus:
function( atlas_test_lvl1_trigger_menu menu )
   atlas_add_test( "L1_${menu}"
                   SCRIPT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateL1MenuMT.sh
                   -r ${CMAKE_PROJECT_VERSION} ${menu} ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu} )
endfunction()

atlas_test_lvl1_trigger_menu( LS2_v1 )
atlas_test_lvl1_trigger_menu( Physics_pp_run3_v1 )
atlas_test_lvl1_trigger_menu( PhysicsP1_pp_run3_v1 )
atlas_test_lvl1_trigger_menu( MC_pp_run3_v1 )
atlas_test_lvl1_trigger_menu( Cosmic_run3_v1 )
atlas_test_lvl1_trigger_menu( PhysicsP1_HI_run3_v1 )
atlas_test_lvl1_trigger_menu( Dev_HI_run3_v1 )
