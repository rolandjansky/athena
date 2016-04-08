################################################################################
# Package: PyKernel
################################################################################

# Declare the package name:
atlas_subdir( PyKernel )

# External dependencies:
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PyKernel
                   src/*.cxx
                   PUBLIC_HEADERS PyKernel
                   INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHON_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( PyKernelDict
                      PyKernel/PyKernelDict.h
                      PyKernel/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} PyKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

