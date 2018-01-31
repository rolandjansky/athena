# Set the project's name and version.
atlas_subdir( HDF5Utils )

# Grab HDF5 from AnalysisBaseExternals.
# first try to use find_package
find_package( HDF5 1.10.1 COMPONENTS CXX )
if (NOT HDF5_FOUND)
  # Assuming we don't find it, use an environment variable. We need
  # this until atlas externals is fixed, see here:
  #
  # https://gitlab.cern.ch/atlas/atlasexternals/merge_requests/209
  #
  message(STATUS "Reading HDF5 Location from $AnalysisBaseExternals_DIR")
  set( H5_LOC $ENV{AnalysisBaseExternals_DIR} )
  set( HDF5_INCLUDE_DIRS "${H5_LOC}/include/")
  set( HDF5_LIB "${H5_LOC}/lib/")
  set( SUFFFIX -shared${CMAKE_SHARED_LIBRARY_SUFFIX})
  set( HDF5_LIBRARIES
    ${HDF5_LIB}/libhdf5_cpp${SUFFFIX} ${HDF5_LIB}/libhdf5${SUFFFIX} )
endif()

# find root
# list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Hist Tree Net Core)

# find boost
set(Boost_USE_STATIC_LIBS ON)
find_package( Boost 1.54.0 REQUIRED COMPONENTS program_options)

# Add the hdf tuple library
atlas_add_library(HDF5Utils
  src/HdfTuple.cxx
  PUBLIC_HEADERS HDF5Utils
  PRIVATE_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} src
  LINK_LIBRARIES ${HDF5_LIBRARIES} ${ZLIB_LIBRARIES})

# build a translation utility
set( lib_sources
  src/copy_root_tree.cxx
  src/get_tree.cxx
  src/tree_copy_opts.cxx
  src/ttree2hdf5.cxx)

atlas_add_executable(ttree2hdf5 ${lib_sources}
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
  LINK_LIBRARIES HDF5Utils ${Boost_LIBRARIES} ${HDF5_LIBRARIES}
  ${ROOT_LIBRARIES})
