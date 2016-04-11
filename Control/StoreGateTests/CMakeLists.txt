################################################################################
# Package: StoreGateTests
################################################################################

# Declare the package name:
atlas_subdir( StoreGateTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaPython
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( StoreGateTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools AthenaBaseComps StoreGateLib SGtests GaudiKernel )

atlas_add_dictionary( StoreGateTestsDict
                      StoreGateTests/StoreGateTestsDict.h
                      StoreGateTests/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools AthenaBaseComps StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_headers( StoreGateTests )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/tests/*.py )

