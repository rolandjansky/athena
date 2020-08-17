# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StoreGateBindings )

# External dependencies:
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS PyROOT Core Tree MathCore Hist RIO pthread ROOTTPython cppyy3_7)

# Component(s) in the package:
atlas_add_library( StoreGateBindings
                   src/*.cxx
                   PUBLIC_HEADERS StoreGateBindings
                   PRIVATE_INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Python_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel DataModelRoot GaudiKernel RootUtils SGTools StoreGateLib )

atlas_add_dictionary( StoreGateBindingsDict
                      src/StoreGateBindingsDict.h
                      StoreGateBindings/selection.xml
                      LINK_LIBRARIES StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
