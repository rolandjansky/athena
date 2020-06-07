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
atlas_install_python_modules( python/*.py )
atlas_install_scripts( bin/*.py python/trigbs_modifyEvent.py python/trigbs_prescaleL1.py python/slimHLTBSFile.py )
atlas_install_data( share/*.ref )

# Unit tests
atlas_add_test( dumpHLTContentInBS_run3
                SCRIPT trigbs_dumpHLTContentInBS_run3.py -n 5 -s 105 --l1 --hlt --hltres --stag --sizes --sizeSummary
                /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data20_test/data_test.00374861.physics_Main.daq.RAW._lb0004._SFO-1._0001.data
                LOG_IGNORE_PATTERN "^----" )
atlas_add_test( flake8
                SCRIPT ${ATLAS_FLAKE8} -- ${CMAKE_CURRENT_SOURCE_DIR}
                POST_EXEC_SCRIPT nopost.sh )
