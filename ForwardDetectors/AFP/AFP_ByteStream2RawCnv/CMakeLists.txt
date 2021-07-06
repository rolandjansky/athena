# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_ByteStream2RawCnv )

# External dependencies:
find_package( tdaq-common )

atlas_add_library( AFP_ByteStream2RawCnvLib
		           src/*.cxx
	               PUBLIC_HEADERS AFP_ByteStream2RawCnv
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaPoolUtilities AFP_RawEv ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel StoreGateLib )

# Component(s) in the package:
atlas_add_component( AFP_ByteStream2RawCnv
                     src/components/*.cxx  AFP_ByteStream2RawCnv/*.h
                     LINK_LIBRARIES AFP_ByteStream2RawCnvLib )
