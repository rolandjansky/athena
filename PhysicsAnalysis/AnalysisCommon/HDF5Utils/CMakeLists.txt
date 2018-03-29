# Set the project's name and version.
atlas_subdir( HDF5Utils )

# Grab HDF5 from AnalysisBaseExternals.
find_package( HDF5 1.10.1 REQUIRED COMPONENTS C CXX )

# find root
find_package(ROOT REQUIRED COMPONENTS RIO Hist Tree Net Core)

# find boost
find_package( Boost 1.54.0 REQUIRED COMPONENTS program_options)

# Add the hdf tuple library
atlas_add_library(HDF5Utils
  Root/HdfTuple.cxx
  PUBLIC_HEADERS HDF5Utils
  PRIVATE_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
  LINK_LIBRARIES ${HDF5_LIBRARIES} ${ZLIB_LIBRARIES})

# build a translation utility
set( _exe_sources
  util/copyRootTree.cxx
  util/getTree.cxx
  util/treeCopyOpts.cxx
  util/ttree2hdf5.cxx)

atlas_add_executable(ttree2hdf5 ${_exe_sources}
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS} util
  ${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
  LINK_LIBRARIES HDF5Utils ${Boost_LIBRARIES} ${HDF5_LIBRARIES}
  ${ROOT_LIBRARIES})

unset(_exe_sources)
