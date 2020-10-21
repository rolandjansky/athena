# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_ModuleDistortions )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

atlas_add_library( SCT_ModuleDistortionsLib
                   SCT_ModuleDistortions/*.h
                   INTERFACE
                   PUBLIC_HEADERS SCT_ModuleDistortions
                   LINK_LIBRARIES GaudiKernel GeoPrimitives TrkParameters Identifier AthenaKernel )

# Component(s) in the package:
atlas_add_component( SCT_ModuleDistortions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel GeoPrimitives Identifier InDetIdentifier InDetReadoutGeometry PathResolver SCT_ModuleDistortionsLib TrkEventPrimitives TrkParameters )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt )

# Test(s) in the package:
atlas_add_test( TestSCT_DistortionsTool
                SCRIPT athena.py --threads=5 SCT_ModuleDistortions/TestSCT_DistortionsTool.py
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )
