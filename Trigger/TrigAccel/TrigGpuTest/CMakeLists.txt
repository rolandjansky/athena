################################################################################
# Package: TrigGpuTest
################################################################################

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

link_libraries(stdc++fs)

# Set the name of the package.
atlas_subdir( TrigGpuTest )

# External dependencies:

find_package(TBB)

atlas_add_executable( trigGpuTest
   src/*.cxx
   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
   LINK_LIBRARIES TrigAccelEvent ${TBB_LIBRARIES} rt dl)
