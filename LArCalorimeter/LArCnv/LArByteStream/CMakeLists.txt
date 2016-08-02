# $Id: CMakeLists.txt 757209 2016-06-23 11:15:56Z krasznaa $
################################################################################
# Package: LArByteStream
################################################################################

# Declare the package name:
atlas_subdir( LArByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloDetDescr
   Calorimeter/CaloIdentifier
   Calorimeter/CaloUtils
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   Event/ByteStreamCnvSvcBase
   Event/ByteStreamData
   Event/EventInfo
   GaudiKernel
   LArCalorimeter/LArIdentifier
   LArCalorimeter/LArROD
   LArCalorimeter/LArRawEvent
   LArCalorimeter/LArRawUtils
   LArCalorimeter/LArRecConditions
   LArCalorimeter/LArRecEvent
   LArCalorimeter/LArRecUtils
   LArCalorimeter/LArTools
   PRIVATE
   Calorimeter/CaloInterface
   Control/CLIDSvc
   Control/SGTools
   DetectorDescription/AtlasDetDescr
   DetectorDescription/Identifier
   Event/ByteStreamCnvSvc
   LArCalorimeter/LArRawConditions )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat eformat_write )

# Component(s) in the package:
atlas_add_library( LArByteStreamLib
   LArByteStream/*.h LArByteStream/*.icc src/*.cxx
   PUBLIC_HEADERS LArByteStream
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier AthenaBaseComps
   AthenaKernel ByteStreamData EventInfo GaudiKernel LArIdentifier LArRawEvent
   LArRecConditions LArRecEvent CaloDetDescrLib CaloUtilsLib StoreGateLib
   ByteStreamCnvSvcBaseLib LArRODLib LArRawUtilsLib LArRecUtilsLib LArToolsLib
   ByteStreamCnvSvcLib
   PRIVATE_LINK_LIBRARIES SGTools AtlasDetDescr Identifier LArRawConditions )

atlas_add_component( LArByteStream
   src/components/*.cxx
   LINK_LIBRARIES LArByteStreamLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )
