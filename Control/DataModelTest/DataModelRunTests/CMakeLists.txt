################################################################################
# Package: DataModelRunTests
################################################################################

# Declare the package name:
atlas_subdir( DataModelRunTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )


function (datamodel_run_test testName)
  cmake_parse_arguments( ARG "" "DEPENDS;COMMAND;ARG" "" ${ARGN} )

  if( ARG_COMMAND )
    set( _command ${ARG_COMMAND} )
  else()
     set( _command athena.py )
  endif()

  if( ARG_ARG )
    set( _arg ${ARG_ARG} )
  else()
    set( _arg DataModelRunTests/${testName}_jo.py )
  endif()

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/datamodel_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/datamodel_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/datamodel_${testName}.sh
                  PROPERTIES TIMEOUT 300
                  ENVIRONMENT "ATLAS_REFERENCE_TAG=DataModelRunTests/DataModelRunTestsReference-01-00-00"
                  POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/post.sh ${testName}"
                   )
  if( ARG_DEPENDS )
    set_tests_properties( DataModelRunTests_${testName}_ctest
                          PROPERTIES DEPENDS DataModelRunTests_${ARG_DEPENDS}_ctest )
  endif()
endfunction (datamodel_run_test)

datamodel_run_test (DataModelTestWrite)
datamodel_run_test (DataModelTestRead DEPENDS DataModelTestWrite)
datamodel_run_test (DataModelTestRead2 DEPENDS DataModelTestRead)
datamodel_run_test (AuxDataTestWrite)
datamodel_run_test (AuxDataTestRead DEPENDS AuxDataTestWrite)
datamodel_run_test (AuxDataTestRead2 DEPENDS AuxDataTestRead)
datamodel_run_test (AuxDataTestRead2b DEPENDS AuxDataTestRead)
datamodel_run_test (AuxDataTestTypelessRead DEPENDS AuxDataTestWrite)
datamodel_run_test (AuxDataTestRead3 DEPENDS AuxDataTestTypelessRead)
datamodel_run_test (xAODTestWrite)
datamodel_run_test (xAODTestRead DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestRead2 DEPENDS xAODTestRead)
datamodel_run_test (xAODTestRead2b DEPENDS xAODTestRead)
datamodel_run_test (xAODTestTypelessRead DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestRead3 DEPENDS xAODTestTypelessRead)
datamodel_run_test (xAODTestReadARA
                    COMMAND python
                    ARG ${CMAKE_CURRENT_SOURCE_DIR}/share/xAODTestReadARA_t.py
                    DEPENDS xAODTestWrite)

# Disabled by default for now: they require HLTResultByteStream,
# which is in AtlasTrigger, not AtlasEvent.
#datamodel_run_test (ByteStreamTestWrite)
#datamodel_run_test (ByteStreamTestRead DEPENDS ByteStreamTestWrite)

