# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaMP )

# External dependencies:
find_package( Boost COMPONENTS filesystem )

# Component(s) in the package:
atlas_add_component( AthenaMP
                     src/*.cxx
                     src/components/*.cxx
                     src/memory-profiler/getPss.cc
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaInterprocess AthenaMPToolsLib GaudiKernel StoreGateLib )

atlas_add_executable( getSharedMemory
                      src/memory-profiler/getSharedMemory.cc
                      src/memory-profiler/getPss.cc )

# Install files from the package:
atlas_install_python_modules( python/*.py python/tests/*.py )
atlas_install_joboptions( share/AthenaMP_EventService.py )

