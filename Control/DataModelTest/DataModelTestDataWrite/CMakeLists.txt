# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelTestDataWrite )

# Component(s) in the package:
atlas_add_library( DataModelTestDataWriteLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelTestDataWrite
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel DataModelAthenaPoolLib DataModelTestDataCommonLib TrigNavigationLib xAODCore
                   PRIVATE_LINK_LIBRARIES AthContainersInterfaces AthenaBaseComps GaudiKernel StoreGateLib TrigSteeringEvent )

atlas_add_component( DataModelTestDataWrite
                     src/components/*.cxx
                     LINK_LIBRARIES DataModelTestDataWriteLib )

atlas_add_dictionary( DataModelTestDataWriteDict
                      DataModelTestDataWrite/DataModelTestDataWriteDict.h
                      DataModelTestDataWrite/selection.xml
                      LINK_LIBRARIES DataModelTestDataWriteLib
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DataVector<DMTest::B> )

atlas_add_sercnv_library ( DataModelTestDataWriteSerCnv
  FILES DataModelTestDataWrite/HVec.h DataModelTestDataWrite/HView.h DataModelTestDataWrite/HAuxContainer.h
  TYPES_WITH_NAMESPACE DMTest::HVec DMTest::HView DMTest::HAuxContainer
  LINK_LIBRARIES DataModelTestDataWriteLib TrigSerializeCnvSvcLib )
