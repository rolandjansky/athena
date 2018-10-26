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
   Control/CLIDSvc
   Control/SGTools
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
   PRIVATE_LINK_LIBRARIES CaloIdentifier SGTools TileCalibBlobObjs
   TileDetDescr )

atlas_add_component( TileByteStream
   src/components/*.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES TileByteStreamLib )


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
