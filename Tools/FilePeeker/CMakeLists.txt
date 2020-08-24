# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FilePeeker )

# Possible extra dependencies:
set( extra_libs )
if( NOT SIMULATIONBASE )
  set( extra_libs ByteStreamEventTPCnv )
endif()

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree RIO)
if( NOT SIMULATIONBASE )
  find_package( tdaq-common COMPONENTS DataReader EventStorage )
endif()

# Component(s) in the package:
atlas_add_executable( PoolFilePeeker
                      src/PoolFilePeeker.cxx
                      src/FileMetaData.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES EventTPCnv IOVDbTPCnv ${extra_libs} ${ROOT_LIBRARIES})


if( NOT SIMULATIONBASE )
  atlas_add_executable( BSFilePeeker
                        src/BSFilePeeker.cxx
                        src/FileMetaData.cxx
                        INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                        LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES})
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
