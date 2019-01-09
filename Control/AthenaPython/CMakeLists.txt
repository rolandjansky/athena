################################################################################
# Package: AthenaPython
################################################################################

# Declare the package name:
atlas_subdir( AthenaPython )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Control/DataModelRoot )

# External dependencies:
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread PyROOT )

# Component(s) in the package:
atlas_add_library( AthenaPython
                   src/*.cxx
                   PUBLIC_HEADERS AthenaPython
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} CxxUtils DataModelRoot )

atlas_add_component( AthenaPythonComps
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                     LINK_LIBRARIES AthenaPython )

atlas_add_dictionary( AthenaPythonDict
                      AthenaPython/AthenaPythonDict.h
                      AthenaPython/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${PYTHON_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel CxxUtils DataModelRoot AthenaPython )

# Install files from the package:
atlas_install_python_modules( python/*.py python/tests )
atlas_install_joboptions( share/*.py share/tests/*.py )

