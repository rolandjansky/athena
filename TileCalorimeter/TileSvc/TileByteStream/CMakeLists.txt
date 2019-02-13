# $Id: CMakeLists.txt 757198 2016-06-23 10:12:56Z krasznaa $
################################################################################
# Package: TileByteStream
################################################################################

# Declare the package name:
atlas_subdir( TileByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthenaKernel
   Event/ByteStreamCnvSvcBase
   Event/ByteStreamData
   GaudiKernel
   TileCalorimeter/TileEvent
   TileCalorimeter/TileIdentifier
   PRIVATE
   Tools/PathResolver
   AtlasTest/TestTools
   Calorimeter/CaloDetDescr
   Calorimeter/CaloIdentifier
   Control/StoreGate
   DetectorDescription/IdDictParser
   Event/ByteStreamCnvSvc
   TileCalorimeter/TileCalib/TileCalibBlobObjs
   TileCalorimeter/TileConditions
   TileCalorimeter/TileDetDescr
   TileCalorimeter/TileL2Algs
   TileCalorimeter/TileRecUtils )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat eformat_write )

# Component(s) in the package:
atlas_add_library( TileByteStreamLib
   TileByteStream/*.h TileByteStream/*.icc src/*.cxx
   PUBLIC_HEADERS TileByteStream
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel
   ByteStreamData GaudiKernel TileEvent TileIdentifier ByteStreamCnvSvcBaseLib
   CaloDetDescrLib StoreGateLib ByteStreamCnvSvcLib TileConditionsLib
   TileL2AlgsLib TileRecUtilsLib
   PRIVATE_LINK_LIBRARIES CaloIdentifier TileCalibBlobObjs
   TileDetDescr )

atlas_add_component( TileByteStream
   src/components/*.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES TileByteStreamLib )


set( extra_patterns "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|TileDetectorTool|Creating TileCondProxyFile|Cache alignment|INFO opening|loading cache|start processing|done processing|WARNING ../MuRcvDigitsCnt|WARNING ../MuRcvRawChCnt|TileL2Builder initialization completed|Initializing TileMuRcvContByteStreamTool|filled inputFileSummary from file|Picked valid file|No SimFlags" )

atlas_add_test( TileDigitsContByteStreamCnv_test
   SCRIPT test/TileDigitsContByteStreamCnv_test.sh
   PROPERTIES TIMEOUT 500
   EXTRA_PATTERNS ${extra_patterns} )

atlas_add_test( TileRawChannelContByteStreamCnv_test
   SCRIPT test/TileRawChannelContByteStreamCnv_test.sh
   PROPERTIES TIMEOUT 500
   EXTRA_PATTERNS ${extra_patterns} )

atlas_add_test( TileBeamElemContByteStreamCnv_test
   SCRIPT test/TileBeamElemContByteStreamCnv_test.sh
   PROPERTIES TIMEOUT 500
   EXTRA_PATTERNS ${extra_patterns} )

atlas_add_test( TileL2ContByteStreamCnv_test
   SCRIPT test/TileL2ContByteStreamCnv_test.sh
   PROPERTIES TIMEOUT 500
   EXTRA_PATTERNS ${extra_patterns} )

atlas_add_test( TileLaserObjByteStreamCnv_test
   SCRIPT test/TileLaserObjByteStreamCnv_test.sh
   PROPERTIES TIMEOUT 500
   EXTRA_PATTERNS ${extra_patterns} )

atlas_add_test( TileMuRcvContByteStreamCnv_test
   SCRIPT test/TileMuRcvContByteStreamCnv_test.sh
   PROPERTIES TIMEOUT 500
   EXTRA_PATTERNS ${extra_patterns} )


atlas_add_test( TileRawChannel2Bytes4_test
                SOURCES test/TileRawChannel2Bytes4_test.cxx
                LINK_LIBRARIES TileByteStreamLib )

atlas_add_test( TileROD_Decoder_test
                SOURCES test/TileROD_Decoder_test.cxx
                LINK_LIBRARIES TileByteStreamLib IdDictParser PathResolver TestTools
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.dump )
