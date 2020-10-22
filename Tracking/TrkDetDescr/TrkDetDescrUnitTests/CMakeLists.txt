# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrUnitTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( TrkDetDescrUnitTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrkDetDescrInterfaces GeoPrimitives TrkDetDescrUtils TrkGeometry TrkSurfaces TrkVolumes TrkNeutralParameters TrkValidationUtils )
