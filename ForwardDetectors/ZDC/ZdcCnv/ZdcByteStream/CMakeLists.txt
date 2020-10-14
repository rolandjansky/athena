# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcByteStream )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( ZdcByteStreamLib
                   src/*.cxx
                   PUBLIC_HEADERS ZdcByteStream
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AsgTools AthenaBaseComps AthContainers ByteStreamData xAODTrigL1Calo ZdcEvent GaudiKernel ByteStreamCnvSvcBaseLib StoreGateLib
                   PRIVATE_LINK_LIBRARIES AthenaKernel ZdcConditions TrigT1Interfaces )

atlas_add_component( ZdcByteStream
                     src/components/*.cxx
                     LINK_LIBRARIES ZdcByteStreamLib )

atlas_add_dictionary( ZdcByteStreamDict
                      ZdcByteStream/ZdcByteStreamDict.h
                      ZdcByteStream/selection.xml
                      LINK_LIBRARIES ZdcByteStreamLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

