# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolTest )

# Component(s) in the package:
atlas_add_component( AthenaPoolTest
                     src/AthenaPoolTestDataReader.cxx
                     src/AthenaPoolTestDataWriter.cxx
                     src/AthenaPoolTestAttrWriter.cxx
                     src/AthenaPoolTestAttrReader.cxx
                     src/InDetRawDataFakeReader.cxx
                     src/InDetRawDataFakeWriter.cxx
                     src/LArCellContFakeWriter.cxx
                     src/LArCellContFakeReader.cxx
                     src/EventInfoWriter.cxx
                     src/EventInfoReader.cxx
                     src/PileUpEventInfoWriter.cxx
                     src/PileUpEventInfoReader.cxx
                     src/TrkTrackFakeReader.cxx
                     src/TrkTrackFakeWriter.cxx
                     src/FakeTrackBuilder.cxx
                     src/RDOReaderDoubleSelector.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaPoolTestData CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities GeoPrimitives Identifier EventInfo GaudiKernel InDetIdentifier InDetReadoutGeometry InDetRawData InDetCompetingRIOsOnTrack InDetPrepRawData InDetRIO_OnTrack LArRecEvent TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkParameters TrkTrack TrkVertexOnTrack EventInfoMgtLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Tests in the package:
function (athenapooltest_run_test testName jo pattern)
  cmake_parse_arguments( ARG "" "" "DEPENDS" ${ARGN} )

  atlas_add_test( ${testName}
                  SCRIPT athena.py AthenaPoolTest/${jo}.py
                  LOG_SELECT_PATTERN ${pattern}
                  LOG_IGNORE_PATTERN "DEBUG Try item:|AthenaAttribute.*DB=|AtlasRelease|Release version"
                  PROPERTIES TIMEOUT 900 )
  if( ARG_DEPENDS )
    set( _alldeps  )
    foreach( _dep ${ARG_DEPENDS} )
      list( APPEND _alldeps AthenaPoolTest_${_dep}_ctest )
    endforeach()
    set_tests_properties( AthenaPoolTest_${testName}_ctest
                          PROPERTIES DEPENDS "${_alldeps}" )
  endif()
endfunction (athenapooltest_run_test)


# Write/read out some simple objects:
athenapooltest_run_test( AthenaPoolTestWrite AthenaPoolTestWrite
                         "^AthenaPoolTestD" )
athenapooltest_run_test( AthenaPoolTestRead AthenaPoolTestRead
                         "AthenaPoolTestD"
                         DEPENDS AthenaPoolTestWrite )

# Write with DoubleEventSelector
athenapooltest_run_test( AthenaPoolTestWriteDoubleSelector AthenaPoolTestWriteDoubleSelector
                         "^DoubleEvent|^SecondaryEvent|^AthenaPoolTest" )
athenapooltest_run_test( AthenaPoolTestReadDoubleSelector AthenaPoolTestReadDoubleSelector
                         "^DoubleEvent|^SecondaryEvent|^AthenaPoolTest"
                         DEPENDS AthenaPoolTestWriteDoubleSelector )

# Add on test of navigation still using simple objects:
# AthenaPoolTestWrite - Step2 - rewrite part in another file
# DoubleEventSelector
athenapooltest_run_test( AthenaPoolTestStep2WriteDoubleSelector AthenaPoolTestStep2WriteDoubleSelector
                         "^DoubleEvent|^SecondaryEvent|^AthenaPoolTest"
                         DEPENDS AthenaPoolTestWriteDoubleSelector )
# normal EventSelector
athenapooltest_run_test( AthenaPoolTestStep2Write AthenaPoolTestStep2Write
                         "^AthenaPoolTestD"
                         DEPENDS AthenaPoolTestWrite )

# Test write/read of indet rdos:
athenapooltest_run_test( InDetRawDataWriter InDetRawDataWriter_jobOptions
                         "InDetRawDataFak" )
athenapooltest_run_test( InDetRawDataRead InDetRawDataReader_jobOptions
                         "InDetRawDataFak"
                         DEPENDS InDetRawDataWriter )

# Test write/read of LAr cell
athenapooltest_run_test( LArCellContWriter LArCellContWriter_jobOptions
                         "LArCellContFake" )
athenapooltest_run_test( LArCellContRead LArCellContReader_jobOptions
                         "LArCellContFake"
                         DEPENDS LArCellContWriter )

# Read two RDOs with the DoubleEventSelector
athenapooltest_run_test( ReadRDODoubleSelector ReadRDODoubleSelector
                         "^DoubleEvent|^SecondaryEvent|^RDOReader"
                         DEPENDS InDetRawDataWriter LArCellContWriter )
athenapooltest_run_test( ReadCombinedRDO ReadCombinedRDO
                         "^DoubleEvent|^SecondaryEvent|^LArCellContFake|^InDetRawDataFak"
                         DEPENDS ReadRDODoubleSelector )

# Test basic overlay algorithms with DoubleEventSelector
athenapooltest_run_test( DoubleEventSelectorOverlayTest DoubleEventSelectorOverlayTest
                         "^DoubleEvent|^SecondaryEvent|^AthenaPoolAddr|^Stream1|Found HelperTools" )

# AthenaMT test for DoubleEventSelector
atlas_add_test( DoubleEventSelectorOverlayTestMT
                SCRIPT athena.py --threads=4 -c \"outputFile=\'OverlayRDOMT.root\'\" AthenaPoolTest/DoubleEventSelectorOverlayTest.py
                LOG_SELECT_PATTERN "^DoubleEvent.*WARNING|ERROR|Terminated successfully"
                PROPERTIES TIMEOUT 900 )

# Check for adding DetDescr tags to event info object:
athenapooltest_run_test( EventInfoWrite EventInfoWrite
                         "^TagInfoMgr|^EventInfoWriter" )
athenapooltest_run_test( EventInfoWriteStep2 EventInfoWriteStep2
                         "^TagInfoMgr|^EventInfoWriter"
                         DEPENDS EventInfoWrite )
athenapooltest_run_test( EventInfoWriteStep3 EventInfoWriteStep3
                         "^TagInfoMgr|^EventInfoWriter"
                         DEPENDS EventInfoWriteStep2 )
athenapooltest_run_test( EventInfoWriteStep4 EventInfoWriteStep4
                         "^TagInfoMgr|^EventInfoWriter"
                         DEPENDS EventInfoWriteStep3 )
athenapooltest_run_test( EventInfoWriteStep5 EventInfoWriteStep5
                         "^TagInfoMgr|^EventInfoWriter"
                         DEPENDS EventInfoWriteStep4 )
athenapooltest_run_test( EventInfoRead EventInfoRead
                         "^TagInfoMgr|^EventInfoWriter"
                         DEPENDS EventInfoWriteStep5 )
