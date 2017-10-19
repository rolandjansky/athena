################################################################################
# Package: ALP
# Author: Sami Kama 2017
################################################################################

# Declare the package name:
atlas_subdir( ALP )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
#                          PRIVATE
                          )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( PythonLibs )
find_package (ROOT COMPONENTS Core Hist )
find_package( tdaq-common COMPONENTS ers eformat eformat_write hltinterface DataWriter DataReader )
# Component(s) in the package:
atlas_add_library( ALP
                   src/*.cxx 
                   PUBLIC_HEADERS ALP
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} 
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel 
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

