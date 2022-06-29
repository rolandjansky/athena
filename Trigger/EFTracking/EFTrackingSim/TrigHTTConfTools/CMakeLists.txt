# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHTTConfTools )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Physics )

# Component(s) in the package:
atlas_add_library( TrigHTTConfToolsLib
   src/*.cxx TrigHTTConfTools/*.h
   PUBLIC_HEADERS TrigHTTConfTools
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib
   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigHTTObjectsLib )

atlas_add_component( TrigHTTConfTools
   src/components/*.cxx
   LINK_LIBRARIES TrigHTTConfToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/HTTInputTestSetup.sh )

# Tests in the package:
atlas_add_test( HTTRegionSlices_test
   SOURCES        test/HTTRegionSlices_test.cxx
   INCLUDE_DIRS   ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TrigHTTConfToolsLib )

atlas_add_test( HTTEventSelectionSvc_test
                SCRIPT python -m TrigHTTConfTools.HTTConfigCompInit
                POST_EXEC_SCRIPT noerror.sh )
