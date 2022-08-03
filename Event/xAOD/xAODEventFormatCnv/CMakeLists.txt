# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODEventFormatCnv )

# Find the necessary "externals".
find_package( xAODUtilities )

# Set up the "main library" of the package.
atlas_add_component( xAODEventFormatCnv
  src/*.h src/*.cxx src/components/*.cxx
  LINK_LIBRARIES
    GaudiKernel AthenaKernel AthenaBaseComps StoreGateLib xAODEventFormat CxxUtils )

# Set up the test code of the package.
atlas_add_library( xAODEventFormatCnvTestLib
  xAODEventFormatCnv/test/*.h xAODEventFormatCnv/test/versions/*.h
  src/test/A_v1.cxx src/test/AAuxContainer_v1.cxx
  PUBLIC_HEADERS xAODEventFormatCnv
  LINK_LIBRARIES AthContainers xAODCore
  PRIVATE_LINK_LIBRARIES AthContainersInterfaces )

atlas_add_component( xAODEventFormatCnvTest
  src/test/*.h src/test/ACreatorAlg.cxx src/test/EventFormatPrinterAlg.cxx
  LINK_LIBRARIES GaudiKernel AthenaKernel AthenaBaseComps StoreGateLib
  xAODEventFormat xAODEventFormatCnvTestLib xAODMetaData )

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
    GaudiKernel
    AthenaKernel
    AthenaPoolCnvSvcLib
    AthContainers
    xAODEventFormatCnvTestLib )

atlas_add_xaod_smart_pointer_dicts(
  INPUT xAODEventFormatCnv/test/selection.xml
  OUTPUT _selectionFile
  CONTAINERS "xAODMakerTest::AVec_v1" )

atlas_add_dictionary( xAODEventFormatCnvTestDict
  xAODEventFormatCnv/test/xAODEventFormatCnvTestDict.h
  ${_selectionFile}
  LINK_LIBRARIES xAODCore xAODEventFormatCnvTestLib
  EXTRA_FILES src/test/dict/*.cxx )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Install python modules for CA
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the package.
atlas_add_test( Write
  SCRIPT athena.py --threads=8 xAODEventFormatCnv/EventFormatWriteTestJobOptions.py
  POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( Read
  SCRIPT athena.py --threads=8 xAODEventFormatCnv/EventFormatReadTestJobOptions.py
  POST_EXEC_SCRIPT nopost.sh
  DEPENDS Write )

atlas_add_test( ReadWrite
  SCRIPT athena.py --threads=8 xAODEventFormatCnv/EventFormatReadWriteJobOptions.py
  POST_EXEC_SCRIPT nopost.sh
  DEPENDS Write )

atlas_add_test( Content
  SCRIPT test/TestEventFormatContent.py readWriteTestStream0.pool.root EventFormat
  POST_EXEC_SCRIPT nopost.sh
  DEPENDS ReadWrite )
