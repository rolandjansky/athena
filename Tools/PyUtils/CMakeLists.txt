################################################################################
# Package: PyUtils
################################################################################

# Declare the package name:
atlas_subdir( PyUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/RootUtils
                          )

# External dependencies:
find_package( PythonLibs )
find_package( requests )
find_package( six )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py python/AthFile python/scripts python/flake8_atlas )
atlas_install_scripts( bin/acmd.py bin/atl-gen-athena-d3pd-reader bin/checkFile.py bin/checkPlugins.py bin/checkSG.py bin/checkMetaSG.py bin/checkTP.py bin/checkxAOD.py bin/diff-athfile bin/diff-jobo-cfg.py bin/diffConfigs.py bin/diffPoolFiles.py bin/diffTAGTree.py bin/dlldep.py bin/dso-stats.py bin/dump-athfile.py bin/dumpAthfilelite.py bin/filter-and-merge-d3pd.py bin/getMetadata.py bin/get-tag-diff.py bin/gprof2dot bin/issues bin/magnifyPoolFile.py bin/merge-poolfiles.py bin/pep8.py bin/pool_extractFileIdentifier.py bin/pool_insertFileToCatalog.py bin/print_auditor_callgraph.py bin/pyroot.py bin/vmem-sz.py bin/meta-reader.py bin/lstags bin/tree-orderer.py )

# Install flake8 plugins using setuptools.
set( _flake8Dir
   "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/flake8Install" )
set( _flake8Stamp
   "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/flake8.stamp" )
add_custom_command( OUTPUT "${_flake8Stamp}"
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/python/flake8_atlas/setup.py
   COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
   python ${CMAKE_CURRENT_SOURCE_DIR}/python/flake8_atlas/setup.py install
   --root /
   --prefix "${_flake8Dir}"
   --install-lib "${_flake8Dir}/${CMAKE_INSTALL_PYTHONDIR}"
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${_flake8Dir}/"
   "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}"
   COMMAND ${CMAKE_COMMAND} -E touch "${_flake8Stamp}"
   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
   COMMENT "Building/installing flake8..." )
add_custom_target( flake8_atlas ALL
   DEPENDS "${_flake8Stamp}" )
install( DIRECTORY "${_flake8Dir}/"
   DESTINATION .
   USE_SOURCE_PERMISSIONS
   OPTIONAL )
unset( _flake8Dir )
unset( _flake8Stamp )

# Aliases:
atlas_add_alias( checkFile "checkFile.py" )
atlas_add_alias( checkMetaSG "checkMetaSG.py" )
atlas_add_alias( dso-stats "dso-stats.py" )
atlas_add_alias( gen_klass "acmd.py" "gen-klass" )
atlas_add_alias( merge-poolfiles "merge-poolfiles.py" )
atlas_add_alias( diffConfigs "diffConfigs.py" )
atlas_add_alias( filter-and-merge-d3pd "filter-and-merge-d3pd.py" )
atlas_add_alias( avn "avn.py" )
atlas_add_alias( diffPoolFiles "diffPoolFiles.py" )
atlas_add_alias( print_auditor_callgraph "print_auditor_callgraph.py" )
atlas_add_alias( dump-athfile "dump-athfile.py" )
atlas_add_alias( pyroot "pyroot.py" )
atlas_add_alias( diffTAGTree "diffTAGTree.py" )
atlas_add_alias( checkxAOD "checkxAOD.py" )
atlas_add_alias( get-tag-diff "get-tag-diff.py" )
atlas_add_alias( checkSG "checkSG.py" )
atlas_add_alias( diff-jobo-cfg "diff-jobo-cfg.py" )
atlas_add_alias( acmd "acmd.py" )
atlas_add_alias( vmem-sz "vmem-sz.py" )
atlas_add_alias( getMetadata "getMetadata.py" )
atlas_add_alias( meta-reader "meta-reader.py" )
atlas_add_alias( tree-orderer "tree-orderer.py" )

# Tests:
atlas_add_test( RootUtils
   SCRIPT test/test_RootUtils.py
   PROPERTIES TIMEOUT 300
   EXTRA_PATTERNS "Ran 1 test in |CheckABICompatibility|standard library" )

atlas_add_test( flake8_OutputLevel
   SCRIPT flake8 --enable-extensions=ATL900 --select=ATL --stdin-display-name=flake8_OutputLevel.py
   --exit-zero --isolated - < ${CMAKE_CURRENT_SOURCE_DIR}/python/flake8_atlas/test/flake8_OutputLevel.py )

atlas_add_test( flake8_logging
   SCRIPT flake8 --enable-extensions=ATL901 --select=ATL --stdin-display-name=flake8_logging.py
   --exit-zero --isolated - < ${CMAKE_CURRENT_SOURCE_DIR}/python/flake8_atlas/test/flake8_logging.py )

atlas_add_test( flake8_print_statement
   SCRIPT flake8 --select=ATL --stdin-display-name=flake8_print_statement.py
   --exit-zero --isolated - < ${CMAKE_CURRENT_SOURCE_DIR}/python/flake8_atlas/test/flake8_print_statement.py )
