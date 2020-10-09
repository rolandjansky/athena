################################################################################
# Package: TrigByteStreamTools
################################################################################

# Declare the package name:
atlas_subdir( TrigByteStreamTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_dictionary( TrigByteStreamToolsDict
                      TrigByteStreamTools/TrigByteStreamToolsDict.h
                      TrigByteStreamTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/*.py python/trigbs_modifyEvent.py python/trigbs_prescaleL1.py python/slimHLTBSFile.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_data( share/*.ref )

# Unit tests
set( trigbs_testFileRun2 "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1")
set( trigbs_testFileRun3 "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data20_test/data_test.00374861.physics_Main.daq.RAW._lb0004._SFO-1._0001.data")
atlas_add_test( dumpHLTContentInBS_run3
                SCRIPT trigbs_dumpHLTContentInBS_run3.py -n 5 -s 105 --l1 --hlt --hltres --stag --sizes --sizeSummary ${trigbs_testFileRun3}
                LOG_IGNORE_PATTERN "^----" )
atlas_add_test( updateMetadata
                SCRIPT rm -f newMeta.*.data
                       && trigbs_updateBSMetadata.py -n 5 --copyFrom ${trigbs_testFileRun2} --outputName newMeta ${trigbs_testFileRun3}
                       && meta-reader -m full newMeta._0001.data
                LOG_IGNORE_PATTERN "file_guid" )
