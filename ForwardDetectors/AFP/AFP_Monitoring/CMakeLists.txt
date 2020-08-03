# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_Monitoring )

# External dependencies:
find_package( ROOT COMPONENTS Hist )

# Component(s) in the package:
atlas_add_component( AFP_Monitoring
   src/*.cxx
   src/components/*.cxx
   LINK_LIBRARIES ${ROOT_LIBRARIES} AFP_RawEv AthenaBaseComps AthenaMonitoringLib GaudiKernel LWHists LumiBlockCompsLib xAODEventInfo xAODForward )

# Install files from the package:
atlas_install_joboptions( share/*.py )
