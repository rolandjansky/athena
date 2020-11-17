# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( VKalVrtValidation )

# Externals used by the package:
find_package( ROOT COMPONENTS Core  )

# Libraries in the package:
atlas_add_component( VKalVrtValidation
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps JpsiUpsilonToolsLib TrkVKalVrtFitterLib xAODTracking )

# Install files from the package:
atlas_install_joboptions( share/*.py )
