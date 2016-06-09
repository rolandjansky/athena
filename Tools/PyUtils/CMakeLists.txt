################################################################################
# Package: PyUtils
################################################################################

# Declare the package name:
atlas_subdir( PyUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/RootUtils
                          Tools/PyCmt )

# External dependencies:
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py python/AthFile python/scripts )
atlas_install_scripts( bin/abootstrap-wkarea.py bin/acmd.py bin/atl-gen-athena-d3pd-reader bin/avn.py bin/build_cmt_pkg_db.py bin/checkFile.py bin/checkPlugins.py bin/checkSG.py bin/checkMetaSG.py bin/checkTP.py bin/checkTag.py bin/checkxAOD.py bin/cmtClients.py bin/diff-athfile bin/diff-jobo-cfg.py bin/diffConfigs.py bin/diffPoolFiles.py bin/diffTAGTree.py bin/dlldep.py bin/dso-stats.py bin/dump-athfile.py bin/dumpAthfilelite.py bin/filter-and-merge-d3pd.py bin/getMetadata.py bin/gen-typereg-dso.py bin/gen_klass.py bin/get-tag-diff.py bin/gprof2dot bin/issues bin/lstags bin/magnifyPoolFile.py bin/merge-poolfiles.py bin/pep8.py bin/pkgco.py bin/pool_extractFileIdentifier.py bin/pool_insertFileToCatalog.py bin/print_auditor_callgraph.py bin/pyroot.py bin/setupWorkArea.py bin/tabnanny-checker.py bin/tcSubmitTag.py bin/vmem-sz.py )

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
atlas_add_alias( tabnanny-checker "tabnanny-checker.py" )
atlas_add_alias( abootstrap-wkarea "abootstrap-wkarea.py" )
atlas_add_alias( pyroot "pyroot.py" )
atlas_add_alias( checkTag "checkTag.py" )
atlas_add_alias( diffTAGTree "diffTAGTree.py" )
atlas_add_alias( checkxAOD "checkxAOD.py" )
atlas_add_alias( get-tag-diff "get-tag-diff.py" )
atlas_add_alias( tcSubmitTag "tcSubmitTag.py" )
atlas_add_alias( pkgco "pkgco.py" )
atlas_add_alias( checkSG "checkSG.py" )
atlas_add_alias( diff-jobo-cfg "diff-jobo-cfg.py" )
atlas_add_alias( acmd "acmd.py" )
atlas_add_alias( vmem-sz "vmem-sz.py" )
atlas_add_alias( build_cmt_pkg_db "build_cmt_pkg_db.py" )
atlas_add_alias( setupWorkArea "setupWorkArea.py" )
atlas_add_alias( tc-submit-tag "tcSubmitTag.py" )
atlas_add_alias( getMetadata "getMetadata.py" )
