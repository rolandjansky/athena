# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_PAI_Process )

# External dependencies:
find_package( CLHEP )

atlas_add_library( TRT_PAI_ProcessLib
                   TRT_PAI_Process/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_PAI_Process
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( TRT_PAI_Process
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel TRT_PAI_ProcessLib AthenaBaseComps AthenaKernel TRT_ReadoutGeometry )

# Install files from the package:
atlas_install_python_modules( python/*.py )
