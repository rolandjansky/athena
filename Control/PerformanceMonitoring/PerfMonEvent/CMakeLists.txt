# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PerfMonEvent )

# External dependencies:
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# We don't have a direct dependency on boost from this package, but if we
# pick up the dependency implicitly rather than explicitly, we include
# root headers with -I rather than -isystem.
# That exposes us to a warning found by gcc8 in mpl/assert.h.
# [Fixed in boost 1.68; see https://github.com/boostorg/mpl/issues/31.]
find_package( Boost )

# Component(s) in the package:
atlas_add_library( PerfMonEvent
                   src/PyStore.cxx
                   src/PyChrono.cxx
                   src/DataModel.cxx
                   src/MemStatsHooks.cxx
                   src/MallocStats.cxx
                   PUBLIC_HEADERS PerfMonEvent
                   INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Python_LIBRARIES} ${Boost_LIBRARIES} GaudiKernel rt
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( PerfMonEventDict
                      PerfMonEvent/PerfMonEventDict.h
                      PerfMonEvent/selection.xml
                      INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Python_LIBRARIES} ${ROOT_LIBRARIES} rt GaudiKernel PerfMonEvent )

