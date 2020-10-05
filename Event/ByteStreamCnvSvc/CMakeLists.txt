# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamCnvSvc )

# External dependencies:
find_package( Boost COMPONENTS system )
find_package( CORAL COMPONENTS CoralBase )
find_package( tdaq-common COMPONENTS eformat_old eformat_write RawFileName
   DataReader DataWriter )

# Libraries in the package:
atlas_add_library( ByteStreamCnvSvcLib
   ByteStreamCnvSvc/*.h src/*.cxx
   PUBLIC_HEADERS ByteStreamCnvSvc
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AthenaBaseComps ByteStreamData EventInfo GaudiKernel
   ByteStreamCnvSvcBaseLib StoreGateLib rt
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES}
   AthenaKernel SGTools CollectionBase FileCatalog
   AthenaPoolUtilities PersistentDataModel xAODEventInfo xAODTrigger
   ByteStreamCnvSvcLegacy )

atlas_add_component( ByteStreamCnvSvc
   src/components/*.cxx
   LINK_LIBRARIES ByteStreamCnvSvcLib )

# Executables in the package:
atlas_add_executable( AtlFindBSEvent test/AtlFindBSEvent.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES} )

atlas_add_executable( AtlCopyBSEvent test/AtlCopyBSEvent.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES}
   CollectionBase FileCatalog PersistentDataModel )

atlas_add_executable( AtlListBSEvents test/AtlListBSEvents.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES} )

# Test(s) in the package:
atlas_add_test( BSEventSelector
   SCRIPT "athena.py --threads 1 ByteStreamCnvSvc/BSEventSelector_test_jobOptions.py"
   LOG_SELECT_PATTERN "ByteStream.*Svc" )

atlas_add_test( AtlCopyBSEvent1
   PRE_EXEC_SCRIPT "rm -f test.data"
   SCRIPT "AtlCopyBSEvent -e 186882810,187403142,187404922,187419528 -o test.data /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"
   PROPERTIES DEPENDS "ByteStreamCnvSvc_BSEventSelector_ctest")

atlas_add_test( AtlFindBSEvent2
   SCRIPT "AtlFindBSEvent -e 187403142 test.data"
   PROPERTIES DEPENDS "ByteStreamCnvSvc_AtlCopyBSEvent1_ctest")

atlas_add_test( AtlCopyBSEvent3
   PRE_EXEC_SCRIPT "rm -f test_defl.data"
   SCRIPT "AtlCopyBSEvent -d -e 186882810,187403142,187419528 -o test_defl.data test.data"
   PROPERTIES DEPENDS "ByteStreamCnvSvc_AtlCopyBSEvent1_ctest")

atlas_add_test( AtlFindBSEvent4
   SCRIPT "AtlFindBSEvent -e 187403142 test_defl.data"
   LOG_IGNORE_PATTERN "+Timestamp"
   PROPERTIES DEPENDS "ByteStreamCnvSvc_AtlCopyBSEvent3_ctest")

atlas_add_test( AtlCopyBSEvent5
   PRE_EXEC_SCRIPT "rm -f test_infl.data"
   SCRIPT "AtlCopyBSEvent -e 186882810,187403142,187419528 -o test_infl.data test_defl.data"
   PROPERTIES DEPENDS "ByteStreamCnvSvc_AtlCopyBSEvent3_ctest")

atlas_add_test( AtlCopyBSEvent6
   PRE_EXEC_SCRIPT "rm -f empty*.data && python ${CMAKE_CURRENT_SOURCE_DIR}/test/create_empty_bsfile.py"
   SCRIPT "AtlCopyBSEvent -e all -o empty.data empty._0001.data"
   PROPERTIES DEPENDS "ByteStreamCnvSvc_AtlCopyBSEvent5_ctest")

atlas_add_test( ByteStreamConfigTest
   SCRIPT "python -m ByteStreamCnvSvc.ByteStreamConfig"
   POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/catalogBytestreamFiles.sh )
