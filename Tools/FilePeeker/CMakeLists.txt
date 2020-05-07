################################################################################
# Package: FilePeekers
################################################################################

# Declare the package name:
atlas_subdir( FilePeeker )

# Possible extra dependencies:
set( extra_dep )
set( extra_libs )
if( NOT SIMULATIONBASE )
  set( extra_dep Event/ByteStreamEventTPCnv )
  set( extra_libs ByteStreamEventTPCnv )
endif()


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          Event/EventTPCnv
                          Database/IOVDbTPCnv
                          ${extra_dep})


# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO)
if( NOT SIMULATIONBASE )
  find_package( tdaq-common COMPONENTS DataReader EventStorage ers )
endif()

find_package( Boost )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

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
atlas_install_python_modules( python/*.py )
