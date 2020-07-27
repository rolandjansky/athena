# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write )

# Component(s) in the package:
atlas_add_library( TrigT1CaloByteStreamLib
                   TrigT1CaloByteStream/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigT1CaloByteStream
                   LINK_LIBRARIES AsgTools AthContainers GaudiKernel xAODTrigL1Calo )

atlas_add_component( TrigT1CaloByteStream
                     src/*.cxx
                     src/xaod/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamCnvSvcBaseLib ByteStreamData CxxUtils SGTools StoreGateLib TrigT1CaloByteStreamLib TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib TrigT1Interfaces ZdcByteStreamLib )

atlas_add_dictionary( TrigT1CaloByteStreamDict
                      TrigT1CaloByteStream/TrigT1CaloByteStreamDict.h
                      TrigT1CaloByteStream/selection.xml
                      LINK_LIBRARIES TrigT1CaloByteStreamLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
