# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PyUtils )

# External dependencies:
find_package( decorator )
find_package( graphviz )
find_package( requests )
find_package( six )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py python/scripts python/AthFile POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/acmd.py bin/checkFile.py bin/checkPlugins.py
   bin/checkSG.py bin/checkMetaSG.py bin/checkTP.py bin/checkxAOD.py
   bin/diff-athfile bin/diff-jobo-cfg.py bin/diffConfigs.py
   bin/diffPoolFiles.py bin/dlldep.py bin/dso-stats.py bin/dump-athfile.py
   bin/dumpAthfilelite.py bin/filter-and-merge-d3pd.py bin/getMetadata.py
   bin/gprof2dot bin/issues bin/magnifyPoolFile.py bin/merge-poolfiles.py
   bin/pool_extractFileIdentifier.py
   bin/pool_insertFileToCatalog.py bin/print_auditor_callgraph.py bin/pyroot.py
   bin/vmem-sz.py bin/meta-reader.py bin/meta-diff.py bin/tree-orderer.py
   POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Aliases:
atlas_add_alias( checkFile "checkFile.py" )
atlas_add_alias( checkMetaSG "checkMetaSG.py" )
atlas_add_alias( dso-stats "dso-stats.py" )
atlas_add_alias( gen_klass "acmd.py" "gen-klass" )
atlas_add_alias( merge-poolfiles "merge-poolfiles.py" )
atlas_add_alias( diffConfigs "diffConfigs.py" )
atlas_add_alias( filter-and-merge-d3pd "filter-and-merge-d3pd.py" )
atlas_add_alias( diffPoolFiles "diffPoolFiles.py" )
atlas_add_alias( print_auditor_callgraph "print_auditor_callgraph.py" )
atlas_add_alias( dump-athfile "dump-athfile.py" )
atlas_add_alias( pyroot "pyroot.py" )
atlas_add_alias( checkxAOD "checkxAOD.py" )
atlas_add_alias( checkSG "checkSG.py" )
atlas_add_alias( diff-jobo-cfg "diff-jobo-cfg.py" )
atlas_add_alias( acmd "acmd.py" )
atlas_add_alias( vmem-sz "vmem-sz.py" )
atlas_add_alias( getMetadata "getMetadata.py" )
atlas_add_alias( meta-reader "meta-reader.py" )
atlas_add_alias( meta-diff   "meta-diff.py" )
atlas_add_alias( tree-orderer "tree-orderer.py" )

# Tests:
atlas_add_test( RootUtils
   SCRIPT test/test_RootUtils.py
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN "Ran 1 test in |CheckABICompatibility|standard library|import imp|imp module" )

atlas_add_test( fprint_test
                SCRIPT python -m PyUtils.fprint )
