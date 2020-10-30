# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSimpleVisual )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( InDetSimpleVisual
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives Identifier GaudiKernel InDetIdentifier InDetReadoutGeometry StoreGateLib TRT_ReadoutGeometry )

# Install files from the package:
atlas_install_joboptions( share/*.py )
