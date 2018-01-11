################################################################################
# Package: TriggerMenu
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenu )

# Install files from the package:
atlas_install_python_modules( python/*.py python/menu python/l1
python/l1menu python/l1topo python/l1topomenu python/egamma
python/muon python/jet python/bjet python/met python/tau python/afp
python/minbias python/heavyion python/bphysics python/calibcosmicmon
python/test python/combined python/commonUtils python/api )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/generate*Menu.py scripts/menuTestTMC.sh 
scripts/generateUnprescaledLists.py )
atlas_install_xmls( data/*.dtd data/*.xml )

atlas_add_test( generateMenu SCRIPT scripts/testMenu.sh 
                PROPERTIES TIMEOUT 500 
                POST_EXEC_SCRIPT "check_log.pl --config checklogTriggerTest.conf generateMenu.log" 
              )

atlas_add_test ( pyflakesMenu
   SCRIPT scripts/pyflakesMenu.sh
   ENVIRONMENT "PYPATH=${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_SOURCE_DIR}/share ${CMAKE_CURRENT_SOURCE_DIR}/scripts"
)

atlas_add_test ( checkL1Menu SCRIPT scripts/checkL1Menu.sh
   POST_EXEC_SCRIPT "check_log.pl --config checklogTriggerTest.conf checkL1Menu.log"
)

# Function to create lowest-unprescaled lists
function( atlas_run_lowestunprescaled )

   # Don't do anything in release recompilation dryrun mode. In all other
   # modes, proceed as usual.
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Command to build the list of unprescaled triggers. 
   # Algorithm copied from TriggerMenuXML
   add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/unprescaled.stamp
      COMMAND ${CMAKE_COMMAND} -E touch
      ${CMAKE_CURRENT_BINARY_DIR}/unprescaled.attempted.stamp
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_CURRENT_BINARY_DIR}/LowestUnprescaledLists/unprescaled
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateUnprescaledLists.py ${CMAKE_CURRENT_BINARY_DIR}/LowestUnprescaledLists/unprescaled
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/TriggerMenu
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_BINARY_DIR}/LowestUnprescaledLists/unprescaled/
      ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/TriggerMenu/api/
      COMMAND ${CMAKE_COMMAND} -E touch
      ${CMAKE_CURRENT_BINARY_DIR}/unprescaled.stamp
      DEPENDS "Package_$<JOIN:$<TARGET_PROPERTY:ATLAS_PACKAGES_TARGET,ATLAS_PACKAGES>,;Package_>" )

   # Create custom target and add it to package dependencies
   add_custom_target( build_list_unprescaled ALL SOURCES
      ${CMAKE_CURRENT_BINARY_DIR}/unprescaled.stamp )

   # In case the file generation failed, because it wasn't even attempted
   install( CODE "if( NOT EXISTS
                     ${CMAKE_CURRENT_BINARY_DIR}/unprescaled.attempted.stamp )
                     message( WARNING \"Generating trigger list of unprescaled items\"
                              \" during the installation\" )
                     execute_process( COMMAND ${CMAKE_COMMAND} -E touch
                        ${CMAKE_CURRENT_BINARY_DIR}/unprescaled.attempted.stamp )
                     execute_process(
                        COMMAND ${CMAKE_COMMAND} -E make_directory
                        ${CMAKE_CURRENT_BINARY_DIR}/LowestUnprescaledLists/unprescaled )
                     execute_process(
                        COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateUnprescaledLists.py
                        ${CMAKE_CURRENT_BINARY_DIR}/LowestUnprescaledLists/unprescaled )
                  endif()" )

   # Install the generated PYTHON files. Note that this installation rule is
   # executed after the previous code. So by this time the files should be
   # in place, if they could be produced.
   install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/LowestUnprescaledLists/unprescaled/
      DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/TriggerMenu
      USE_SOURCE_PERMISSIONS
      FILES_MATCHING PATTERN "list*.py" )

   # Create a target that will depend on all the other targets, and will
   # print the "right message" at the end of the build.
   if( NOT TARGET TriggerMenuMain )
      add_custom_target( TriggerMenuMain ALL
         COMMAND ${CMAKE_COMMAND} -E echo
         "TriggerMenu: Package build succeeded" )
   endif()
   add_dependencies( TriggerMenuMain build_list_unprescaled )

endfunction ( atlas_run_lowestunprescaled )

atlas_run_lowestunprescaled ( )
