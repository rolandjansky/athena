# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamCnvSvcBase )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat eformat_write )

# Component(s) in the package:
atlas_add_library( ByteStreamCnvSvcBaseLib
                   src/*.cxx
                   PUBLIC_HEADERS ByteStreamCnvSvcBase
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamData GaudiKernel StoreGateLib SGtests ByteStreamData_test
                   PRIVATE_LINK_LIBRARIES SGTools )

atlas_add_component( ByteStreamCnvSvcBase
                     src/components/*.cxx
                     LINK_LIBRARIES ByteStreamCnvSvcBaseLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Tests in the package:
atlas_add_test( ROBDataProviderSvcMT
		SCRIPT test/test_ROBDataProviderSvcMT.sh
		PROPERTIES TIMEOUT 1200 )
