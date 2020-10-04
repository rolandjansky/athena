# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODEventFormatCnv )

find_package( xAODUtilities )

atlas_add_component( xAODEventFormatCnv
  src/*.h src/*.cxx src/components/*.cxx
  LINK_LIBRARIES
    GaudiKernel AthenaKernel AthenaBaseComps StoreGateLib xAODEventFormat )

atlas_add_component( xAODEventFormatCnvTest
  src/test/*.h src/test/*.cxx
  LINK_LIBRARIES
    GaudiKernel
    AthenaBaseComps
    AthContainers
    AthContainersInterfaces
    StoreGateLib
    xAODCore
    xAODEventFormat )

atlas_add_library( xAODEventFormatCnvTestLib
  src/test/*.h src/test/*.cxx
  PUBLIC_HEADERS xAODEventFormatCnv
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES
    GaudiKernel
    AthenaKernel
    AthContainers
    AthContainersInterfaces
    xAODCore
    xAODEventFormat
  PRIVATE_LINK_LIBRARIES AthenaBaseComps StoreGateLib )


atlas_add_poolcnv_library( xAODEventFormatCnvTestAthenaPoolCnv
  src/test/cnv/*.h src/test/cnv/*.cxx
  FILES
    xAODEventFormatCnv/test/A.h
    xAODEventFormatCnv/test/AVec.h
    xAODEventFormatCnv/test/AAuxContainer.h
  TYPES_WITH_NAMESPACE
    xAODMakerTest::A
    xAODMakerTest::AVec
    xAODMakerTest::AAuxContainer
  CNV_PFX xAODMakerTest
  LINK_LIBRARIES
    AthenaPoolCnvSvcLib
    AthenaPoolUtilities
    AthContainers
    xAODEventFormatCnvTestLib )

atlas_add_dictionary( xAODEventFormatCnvTestDict
  xAODEventFormatCnv/test/xAODEventFormatCnvTestDict.h
  xAODEventFormatCnv/test/selection.xml
  ${_selectionFile}
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES xAODCore xAODEventFormatCnvTestLib
  EXTRA_FILES src/test/dict/*.cxx )

atlas_add_xaod_smart_pointer_dicts(
  INPUT xAODEventFormatCnv/test/selection.xml
  OUTPUT _selectionFile
  CONTAINERS "xAODMakerTest::AVec_v1" )


# Install files from the package.
atlas_install_joboptions( share/*.py )

atlas_depends_on_subdirs(
  PUBLIC
  PRIVATE AtlasTest/TestTools )

# Test(s) in the package.
atlas_add_test( Write
  SCRIPT athena.py --threads=8 xAODEventFormatCnv/EventFormatWriteTestJobOptions.py )

atlas_add_test( Read
  SCRIPT athena.py --threads=8 xAODEventFormatCnv/EventFormatReadTestJobOptions.py
  PROPERTIES DEPENDS "xAODEventFormatCnv_Write_ctest" )

atlas_add_test( ReadWrite
  SCRIPT athena.py --threads=8 xAODEventFormatCnv/EventFormatReadWriteJobOptions.py
  PROPERTIES DEPENDS "xAODEventFormatCnv_Write_ctest" )

atlas_add_test( Content
  SCRIPT test/TestEventFormatContent.py readWriteTestStream0.pool.root EventFormat
  PROPERTIES DEPENDS "xAODEventFormatCnv_ReadWrite_ctest" )
