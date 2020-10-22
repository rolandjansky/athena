# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExUnitTests )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( TrkExUnitTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel GeoPrimitives TrkEventPrimitives TrkExInterfaces TrkExUtils TrkGeometry TrkNeutralParameters TrkParameters TrkValInterfaces )
