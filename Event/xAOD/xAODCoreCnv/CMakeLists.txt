# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( xAODCoreCnv )

# External(s).
find_package( ROOT COMPONENTS Core )

# Component(s) in the package.
atlas_add_library( xAODCoreCnvLib
   xAODCoreCnv/*.h
   INTERFACE
   PUBLIC_HEADERS xAODCoreCnv
   LINK_LIBRARIES GaudiKernel )

atlas_add_component( xAODCoreCnv
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaBaseComps
   AthenaKernel SGTools xAODCore GaudiKernel xAODCoreCnvLib )

# Install files from the package.
atlas_install_joboptions( share/*.py )
