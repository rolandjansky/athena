################################################################################
# Package: AFP_ByteStream2RawCnv
################################################################################
  
# Declare the package name:
atlas_subdir( AFP_ByteStream2RawCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthenaBaseComps                          
			  Control/DataModel
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          ForwardDetectors/AFP/AFP_RawEv
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps )

# External dependencies:
find_package( tdaq-common )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

atlas_add_library( AFP_ByteStream2RawCnvLib
		   src/*.cxx
	           PUBLIC_HEADERS AFP_ByteStream2RawCnv
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}  ${CORAL_LIBRARIES} AthenaBaseComps AthContainers AFP_RawEv ByteStreamCnvSvcBaseLib )

# Component(s) in the package:
atlas_add_component( AFP_ByteStream2RawCnv
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CORAL_LIBRARIES}  AthenaBaseComps AthContainers AFP_RawEv AFP_ByteStream2RawCnvLib )

