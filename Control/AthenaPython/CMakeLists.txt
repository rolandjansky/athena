# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPython )

# External dependencies:
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS Core PyROOT ROOTTPython
   cppyy${Python_VERSION_MAJOR}_${Python_VERSION_MINOR} )

# Component(s) in the package:
atlas_add_library( AthenaPython
   AthenaPython/*.h src/*.cxx
   PUBLIC_HEADERS AthenaPython
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Python_INCLUDE_DIRS}
   LINK_LIBRARIES AthenaBaseComps GaudiKernel CxxUtils
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${Python_LIBRARIES} DataModelRoot
   RootUtilsPyROOT )

atlas_add_component( AthenaPythonComps
   src/components/*.cxx
   LINK_LIBRARIES AthenaPython )

atlas_add_dictionary( AthenaPythonDict
   AthenaPython/AthenaPythonDict.h
   AthenaPython/selection.xml
   LINK_LIBRARIES GaudiKernel AthenaKernel AthenaBaseComps AthenaPython )

# Install files from the package:
atlas_install_python_modules( python/*.py python/tests
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py share/tests/*.py )
