# $Id: CMakeLists.txt 728987 2016-03-10 16:51:52Z krasznaa $
################################################################################
# Package: HLToks
################################################################################

# Declare the package name:
atlas_subdir( HLToks )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          HLT/HLTUtils )

# We need the tdaq python modules for the following to work:
find_package( tdaq )

# HLT-Environment generation. First, set up the rules to generate this file
# after all the other packages in the project are done.
set( _hlt_env_xml
   ${CMAKE_DATA_OUTPUT_DIRECTORY}/daq/segments/HLT-Environment.data.xml )
add_custom_command( OUTPUT ${_hlt_env_xml}
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_DATA_OUTPUT_DIRECTORY}/daq/segments
   COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh make_hlt_env.py
   ${_hlt_env_xml}
   DEPENDS
   "Package_$<JOIN:$<TARGET_PROPERTY:ATLAS_PACKAGES_TARGET,ATLAS_PACKAGES>,;Package_>"
   )
add_custom_target( HLToks_build_env ALL SOURCES ${_hlt_env_xml} )

# This rule makes sure that if the generation of the environment file failed,
# because some other package in the project failed, that we would attempt
# generating this file at least in the installation step.
install( CODE "if( NOT EXISTS ${_hlt_env_xml} )
                  message( WARNING \"Generating ${_hlt_env_xml} as part of \"
                           \"the installation step\" )
                  execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory
                                   ${CMAKE_DATA_OUTPUT_DIRECTORY}/daq/segments )
                  execute_process( COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
                                   make_hlt_env.py ${_hlt_env_xml} )
               endif()" )

# Note that this installation command is only attempted after the previous
# one is already done. So it should only fail if the file generation has
# *really* failed.
install( FILES ${_hlt_env_xml}
   DESTINATION ${CMAKE_INSTALL_DATADIR}/daq/segments
   OPTIONAL )

# HLT_SW_Repository generation. First, set up the rules to generate this file
# after all the other packages in the project are done.
set( _hlt_rep_xml
   ${CMAKE_DATA_OUTPUT_DIRECTORY}/daq/sw/HLT_SW_Repository.data.xml )
add_custom_command( OUTPUT ${_hlt_rep_xml}
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_DATA_OUTPUT_DIRECTORY}/daq/sw
   COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_DATA_OUTPUT_DIRECTORY}
   ${CMAKE_BINARY_DIR}/atlas_build_run.sh make_hlt_rep.py
   COMMAND ${CMAKE_COMMAND} -E rename
   ${CMAKE_DATA_OUTPUT_DIRECTORY}/HLT_SW_Repository.data.xml
   ${_hlt_rep_xml} )
add_custom_target( HLToks_build_swrep ALL SOURCES ${_hlt_rep_xml} )
add_dependencies( HLToks_build_swrep HLToks_build_env )

# This rule makes sure that if the generation of the software repository file
# failed, because some other package in the project failed, that we would
# attempt generating this file at least in the installation step.
install( CODE "if( NOT EXISTS ${_hlt_rep_xml} )
                  message( WARNING \"Generating ${_hlt_rep_xml} as part of \"
                           \"the installation step\" )
                  execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory
                                   ${CMAKE_DATA_OUTPUT_DIRECTORY}/daq/sw )
                  execute_process( COMMAND ${CMAKE_COMMAND} -E chdir
                                   ${CMAKE_DATA_OUTPUT_DIRECTORY}
                                   ${CMAKE_BINARY_DIR}/atlas_build_run.sh
                                   make_hlt_rep.py )
                  execute_process( COMMAND ${CMAKE_COMMAND} -E rename
                       ${CMAKE_DATA_OUTPUT_DIRECTORY}/HLT_SW_Repository.data.xml
                                   ${_hlt_rep_xml} )
               endif()" )

# Note that this installation command is only attempted after the previous
# one is already done. So it should only fail if the file generation has
# *really* failed.
install( FILES ${_hlt_rep_xml}
   DESTINATION ${CMAKE_INSTALL_DATADIR}/daq/sw
   OPTIONAL )

# Create a custom target that prints the right message when everything
# succeeded:
add_custom_target( HLToksMain ALL
   COMMAND ${CMAKE_COMMAND} -E echo "HLToks: Package build succeeded" )
add_dependencies( HLToksMain HLToks_build_env HLToks_build_swrep )
