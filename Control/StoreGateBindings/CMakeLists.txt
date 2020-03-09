################################################################################
# Package: StoreGateBindings
################################################################################

# Declare the package name:
atlas_subdir( StoreGateBindings )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          Control/DataModelRoot
                          Control/SGTools
                          Control/StoreGate
                          Control/RootUtils
                          GaudiKernel )

# External dependencies:
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS PyROOT Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( StoreGateBindings
                   src/*.cxx
                   PUBLIC_HEADERS StoreGateBindings
                   PRIVATE_INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests RootUtils
                   PRIVATE_LINK_LIBRARIES ${Python_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel DataModelRoot SGTools GaudiKernel )

atlas_add_dictionary( StoreGateBindingsDict
                      src/StoreGateBindingsDict.h
                      StoreGateBindings/selection.xml
                      INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Python_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel DataModelRoot SGTools StoreGateLib SGtests GaudiKernel StoreGateBindings )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
