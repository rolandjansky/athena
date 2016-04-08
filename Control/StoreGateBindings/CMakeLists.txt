################################################################################
# Package: StoreGateBindings
################################################################################

# Declare the package name:
atlas_subdir( StoreGateBindings )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/DataModelRoot
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( PythonLibs )
find_package( ROOT COMPONENTS PyROOT Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( StoreGateBindings
                   src/*.cxx
                   PUBLIC_HEADERS StoreGateBindings
                   PRIVATE_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils DataModelRoot SGTools GaudiKernel )

atlas_add_dictionary( StoreGateBindingsDict
                      src/StoreGateBindingsDict.h
                      StoreGateBindings/selection.xml
                      INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel CxxUtils DataModelRoot SGTools StoreGateLib SGtests GaudiKernel StoreGateBindings )

# Install files from the package:
atlas_install_python_modules( python/*.py )

