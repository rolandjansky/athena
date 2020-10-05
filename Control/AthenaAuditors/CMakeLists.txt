# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaAuditors )

# External dependencies:
find_package( Boost )
find_package( gdb )
find_package( gperftools )
find_package( libunwind )

# Component(s) in the package:
atlas_add_component( AthenaAuditors
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GDB_INCLUDE_DIRS} ${LIBUNWIND_INCLUDE_DIRS} ${GPERFTOOLS_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GDB_LIBRARIES} ${LIBUNWIND_LIBRARIES} ${CMAKE_DL_LIBS}
   AthenaBaseComps AthenaKernel CxxUtils GaudiKernel )
