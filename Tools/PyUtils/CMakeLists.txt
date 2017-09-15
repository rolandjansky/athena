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
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py python/AthFile python/scripts )
atlas_install_scripts( bin/acmd.py bin/atl-gen-athena-d3pd-reader bin/checkFile.py bin/checkPlugins.py bin/checkSG.py bin/checkMetaSG.py bin/checkTP.py bin/checkTag.py bin/checkxAOD.py bin/diff-athfile bin/diff-jobo-cfg.py bin/diffConfigs.py bin/diffPoolFiles.py bin/diffTAGTree.py bin/dlldep.py bin/dso-stats.py bin/dump-athfile.py bin/dumpAthfilelite.py bin/filter-and-merge-d3pd.py bin/getMetadata.py bin/gen_klass.py bin/get-tag-diff.py bin/gprof2dot bin/issues bin/magnifyPoolFile.py bin/merge-poolfiles.py bin/pep8.py bin/pool_extractFileIdentifier.py bin/pool_insertFileToCatalog.py bin/print_auditor_callgraph.py bin/pyroot.py bin/vmem-sz.py )

# Aliases:
atlas_add_alias( checkFile "checkFile.py" )
atlas_add_alias( checkMetaSG "checkMetaSG.py" )
atlas_add_alias( dso-stats "dso-stats.py" )
atlas_add_alias( gen_klass "gen_klass.py" )
atlas_add_alias( merge-poolfiles "merge-poolfiles.py" )
atlas_add_alias( diffConfigs "diffConfigs.py" )
atlas_add_alias( filter-and-merge-d3pd "filter-and-merge-d3pd.py" )
atlas_add_alias( avn "avn.py" )
atlas_add_alias( diffPoolFiles "diffPoolFiles.py" )
atlas_add_alias( print_auditor_callgraph "print_auditor_callgraph.py" )
atlas_add_alias( dump-athfile "dump-athfile.py" )
atlas_add_alias( pyroot "pyroot.py" )
atlas_add_alias( checkTag "checkTag.py" )
atlas_add_alias( diffTAGTree "diffTAGTree.py" )
atlas_add_alias( checkxAOD "checkxAOD.py" )
atlas_add_alias( get-tag-diff "get-tag-diff.py" )
atlas_add_alias( checkSG "checkSG.py" )
atlas_add_alias( diff-jobo-cfg "diff-jobo-cfg.py" )
atlas_add_alias( acmd "acmd.py" )
atlas_add_alias( vmem-sz "vmem-sz.py" )
atlas_add_alias( getMetadata "getMetadata.py" )
