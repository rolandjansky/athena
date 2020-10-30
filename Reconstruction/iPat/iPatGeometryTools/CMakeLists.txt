# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatGeometryTools )

# Component(s) in the package:
atlas_add_component( iPatGeometryTools
                     src/DetectorSelection.cxx
                     src/LayerAllocator.cxx
                     src/LayerNumberAllocator.cxx
                     src/SiDetectorLocator.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel iPatGeometry iPatInterfaces InDetIdentifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_joboptions( share/*.py )
