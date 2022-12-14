# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_library( LArByteStreamLib
   LArByteStream/*.h LArByteStream/*.icc src/*.cxx
   PUBLIC_HEADERS LArByteStream
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier AthenaBaseComps
   AthenaKernel xAODEventInfo ByteStreamData GaudiKernel LArIdentifier LArRawEvent
   LArRecConditions LArRecEvent CaloDetDescrLib CaloUtilsLib StoreGateLib
   ByteStreamCnvSvcBaseLib LArRawUtilsLib LArRecUtilsLib LArCablingLib
   ByteStreamCnvSvcLib CaloConditions CxxUtils
   PRIVATE_LINK_LIBRARIES AtlasDetDescr CaloInterfaceLib Identifier LArRawConditions )

atlas_add_component( LArByteStream
   src/components/*.cxx
   LINK_LIBRARIES LArByteStreamLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.txt share/*.py )
