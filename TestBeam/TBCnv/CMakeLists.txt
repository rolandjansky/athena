# $Id: CMakeLists.txt 757192 2016-06-23 09:39:00Z krasznaa $
################################################################################
# Package: TBCnv
################################################################################

# Declare the package name:
atlas_subdir( TBCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloIdentifier
   Control/AthenaBaseComps
   Control/StoreGate
   Event/ByteStreamCnvSvc
   Event/ByteStreamCnvSvcBase
   Event/ByteStreamData
   Event/EventInfo
   GaudiKernel
   LArCalorimeter/LArIdentifier
   LArCalorimeter/LArTools
   TestBeam/TBEvent
   PRIVATE
   DetectorDescription/Identifier
   LArCalorimeter/LArRawEvent )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat eformat_write )

# Component(s) in the package:
atlas_add_library( TBCnvLib
   TBCnv/*.h TBCnv/*.icc src/*.cxx
   PUBLIC_HEADERS TBCnv
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier AthenaBaseComps
   ByteStreamData EventInfo GaudiKernel LArIdentifier TBEvent StoreGateLib
   ByteStreamCnvSvcLib ByteStreamCnvSvcBaseLib LArToolsLib
   PRIVATE_LINK_LIBRARIES Identifier LArRawEvent )

atlas_add_component( TBCnv
   src/components/*.cxx
   LINK_LIBRARIES TBCnvLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
