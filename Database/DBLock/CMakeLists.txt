# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

atlas_subdir( DBLock )
find_package( Threads )

atlas_add_library ( DBLock
  src/*.cxx
  PUBLIC_HEADERS DBLock
  LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )

atlas_add_test( DBLock_test
                SOURCES test/DBLock_test.cxx
                LINK_LIBRARIES DBLock )
