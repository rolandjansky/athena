################################################################################
# Package: HeapMon
################################################################################

# Declare the package name:
atlas_subdir( HeapMon )

# External dependencies:
find_package( PythonLibs )
find_package( ROOT COMPONENTS RIO Tree Core MathCore Hist pthread )
find_package( gperftools )

# Component(s) in the package:
atlas_add_executable( MemoryScanner
                      src/MemoryScanner.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${GPERFTOOLS_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} ${GPERFTOOLS_LIBRARIES} )

# Install files from the package:
atlas_install_headers( HeapMon )
atlas_install_python_modules( python/*.py )

