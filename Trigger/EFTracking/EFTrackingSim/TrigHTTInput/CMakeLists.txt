################################################################################
# Package: TrigHTTInput
################################################################################

atlas_subdir( TrigHTTInput )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )


# Component(s) in the package:
atlas_add_library(
  TrigHTTInputLib           src/*.cxx TrigHTTInput/*.h 
  PUBLIC_HEADERS            TrigHTTInput
  INCLUDE_DIRS              ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES            ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
                            TrigHTTBanksLib TrigHTTConfToolsLib TrigHTTMapsLib TrigHTTObjectsLib 
                            TrigHTTSGInputLib
)

atlas_add_component(
  TrigHTTInput              src/components/*.cxx
  LINK_LIBRARIES            TrigHTTInputLib 
)

# Install files from the package:
atlas_install_joboptions( share/*.py test/*.py)
atlas_install_scripts( scripts/*)
atlas_install_python_modules( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 ) 



atlas_add_test(             testReadRawHit
  SCRIPT                    test/test_TrigHTTReadRawHitsWrapperAlg.sh
  LOG_SELECT_PATTERN        "HTTRawHitsWrapp"
  PRIVATE_WORKING_DIRECTORY
)


atlas_add_test(             testRawToLogical
  SCRIPT                    test/test_TrigHTTRawToLogicalWrapperAlg.sh 
  LOG_SELECT_PATTERN        "HTTReadLogical"
  PRIVATE_WORKING_DIRECTORY
)

atlas_add_test(             testDumpOutputStat
  SCRIPT                    test/test_TrigHTTDumpOutputStatAlg.sh  
  LOG_SELECT_PATTERN        "HTTDumpOutputSt"
  PRIVATE_WORKING_DIRECTORY
)

