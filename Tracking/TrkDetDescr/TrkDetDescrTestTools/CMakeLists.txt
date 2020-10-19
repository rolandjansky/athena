# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrTestTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkDetDescrTestTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces EventPrimitives TrkDetDescrUtils TrkGeometry TrkSurfaces TrkVolumes TrkEventPrimitives TrkValidationUtils )
