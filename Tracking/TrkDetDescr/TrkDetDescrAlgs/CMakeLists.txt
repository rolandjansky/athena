# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkDetDescrAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel StoreGateLib TrkDetDescrUtils TrkDetDescrInterfaces TrkGeometry TrkVolumes TrkNeutralParameters TrkExInterfaces )

# Install files from the package:
atlas_install_headers( TrkDetDescrAlgs )

