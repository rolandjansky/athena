# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelTestDataRead )

# Component(s) in the package:
atlas_add_library( DataModelTestDataReadLib
                   src/*.cxx
                   PUBLIC_HEADERS DataModelTestDataRead
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel DataModelAthenaPoolLib DataModelTestDataCommonLib TrigNavigationLib xAODCore
                   PRIVATE_LINK_LIBRARIES AthContainersInterfaces AthenaBaseComps CxxUtils GaudiKernel StoreGateLib TrigSteeringEvent )

atlas_add_component( DataModelTestDataRead
                     src/components/*.cxx
                     LINK_LIBRARIES DataModelTestDataReadLib )

atlas_add_dictionary( DataModelTestDataReadDict
                      DataModelTestDataRead/DataModelTestDataReadDict.h
                      DataModelTestDataRead/selection.xml
                      LINK_LIBRARIES DataModelTestDataReadLib
                      NO_ROOTMAP_MERGE
                      EXTRA_FILES src/dict/*.cxx
                      ELEMENT_LINKS DataVector<DMTest::B> )

atlas_add_sercnv_library ( DataModelTestDataReadSerCnv
  FILES DataModelTestDataRead/HVec.h DataModelTestDataRead/HView.h DataModelTestDataRead/HAuxContainer.h
  TYPES_WITH_NAMESPACE DMTest::HVec DMTest::HView DMTest::HAuxContainer
  LINK_LIBRARIES DataModelTestDataReadLib TrigSerializeCnvSvcLib )
