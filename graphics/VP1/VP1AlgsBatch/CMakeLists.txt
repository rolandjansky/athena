# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1AlgsBatch )

# Component(s) in the package:
atlas_add_component( VP1AlgsBatch
   src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel StorageSvc xAODEventInfo PathResolver
   VP1UtilsBase )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/vp1batch )
