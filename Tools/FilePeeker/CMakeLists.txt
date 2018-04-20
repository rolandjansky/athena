################################################################################
# Package: FilePeekers
################################################################################

# Declare the package name:
atlas_subdir( FilePeeker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
			  Event/EventTPCnv
			  Database/IOVDbTPCnv
			  Event/ByteStreamEventTPCnv)


# External dependencies:
find_package( ROOT COMPONENTS Core Tree )
find_package( tdaq-common COMPONENTS DataReader )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:

atlas_add_executable( PoolFilePeeker
		      src/PoolFilePeeker.cxx
		      src/FileMetaData.cxx 
		      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                      LINK_LIBRARIES  ${ROOT_LIBRARIES}  xAODEventInfo EventTPCnv EventTPCnvDict)


atlas_add_executable( BSFilePeeker 
		      src/BSFilePeeker.cxx
		      src/FileMetaData.cxx 
		      INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
		      LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES})

# Install files from the package:
atlas_install_python_modules( python/*.py )

