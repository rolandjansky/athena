# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloCondBlobObjs )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( CaloCondBlobObjs
                   src/*.cxx
                   PUBLIC_HEADERS CaloCondBlobObjs
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( CaloCondBlobObjsDict
                      CaloCondBlobObjs/CaloCondBlobObjsDict.h
                      CaloCondBlobObjs/selection.xml
                      LINK_LIBRARIES CaloCondBlobObjs )

atlas_add_test( CaloCondBlobBase_test
  SOURCES
  test/CaloCondBlobBase_test.cxx src/CaloCondBlobBase.cxx
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} CaloCondBlobObjs CxxUtils
  POST_EXEC_SCRIPT nopost.sh
)
