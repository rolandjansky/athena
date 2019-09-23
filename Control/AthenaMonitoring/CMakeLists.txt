################################################################################
# Package: AthenaMonitoring
################################################################################

# Declare the package name:
atlas_subdir( AthenaMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          LumiBlock/LumiBlockComps
                          LumiBlock/LumiBlockData
                          Trigger/TrigEvent/TrigDecisionInterface
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/SGMon/SGAudCore
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          Event/EventInfo
                          Tools/LWHists
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          AtlasTest/TestTools)

# External dependencies:
find_package( Boost COMPONENTS filesystem system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( AthenaMonitoringLib
                   src/*.cxx
                   src/HistogramFiller/*.cxx
                   PUBLIC_HEADERS AthenaMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel LumiBlockCompsLib LumiBlockData
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel SGAudCore AthenaPoolUtilities EventInfo LWHists )

atlas_add_component( AthenaMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES AthenaMonitoringLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/Run3DQTestingDriver.py )

# Units tests C++:
file( GLOB CXX_TEST_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cxx )
foreach ( test_file ${CXX_TEST_FILES} )
   get_filename_component( name ${test_file} NAME_WE)
   set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_cxx_${name} )
   file( REMOVE_RECURSE ${rundir} )
   file( MAKE_DIRECTORY ${rundir} )
   atlas_add_test( ${name}
      SOURCES ${test_file}
      LINK_LIBRARIES AthenaMonitoringLib GaudiKernel TestTools AthenaKernel
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      POST_EXEC_SCRIPT "true"
         PROPERTIES TIMEOUT 300
         PROPERTIES WORKING_DIRECTORY ${rundir}
   )
endforeach()

# Units tests Python:

file( GLOB PYTHON_TEST_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.py )
foreach ( test_file ${PYTHON_TEST_FILES} )
   get_filename_component( name ${test_file} NAME_WE)
   set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_py_${name} )
   file( REMOVE_RECURSE ${rundir} )
   file( MAKE_DIRECTORY ${rundir} )
   atlas_add_test( ${name}
      SCRIPT python ${test_file}
      POST_EXEC_SCRIPT "true"
         PROPERTIES WORKING_DIRECTORY ${rundir}
   )
endforeach()

# Code quality check
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*.py
   POST_EXEC_SCRIPT nopost.sh )
